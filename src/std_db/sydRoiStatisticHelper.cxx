/*=========================================================================
  Program:   syd

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ===========================================================================**/

// syd
#include "sydRoiStatisticHelper.h"
#include "sydStandardDatabase.h"
#include "sydTagHelper.h"
#include "sydImageCrop.h"
#include "sydImageAnd.h"
#include "sydRoiMaskImageHelper.h"

// itk
#include "itkLabelStatisticsImageFilter.h"

// --------------------------------------------------------------------
syd::RoiStatistic::vector
syd::FindRoiStatistic(syd::Image::pointer image,
                      syd::RoiMaskImage::pointer mask)
{
  typedef odb::query<syd::RoiStatistic> Q;
  Q q = Q::image == image->id;
  if (mask != nullptr) q = q and (Q::mask == mask->id);
  syd::RoiStatistic::vector stats;
  auto db = image->GetDatabase<syd::StandardDatabase>();
  db->Query(stats, q);
  if (mask==nullptr) {
    DD("to test : FindRoiStatistic with mask == nullptr");
    auto stats_copy = stats;
    DDS(stats_copy);
    stats.clear();
    for(auto & s:stats_copy)
      if (s->mask != nullptr) // remove the stat with a mask
        stats.push_back(s);
    DDS(stats);
  }
  return stats;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiStatistic::pointer
syd::NewRoiStatistic(syd::Image::pointer image,
                     syd::RoiMaskImage::pointer mask,
                     syd::Image::pointer mask2,
                     std::string mask_output_filename)
{
  // new
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::RoiStatistic::pointer stat;
  db->New(stat);
  stat->image = image;
  stat->mask = mask;

  // copy the image tags
  syd::AddTag(stat->tags, image->tags);

  // update
  // Get the itk images
  typedef unsigned char MaskPixelType;
  typedef itk::Image<MaskPixelType,3> MaskImageType;
  auto mask2_itk = syd::ReadImage<MaskImageType>(mask2->GetAbsolutePath());
  auto itk_mask = syd::UpdateRoiStatistic(stat, mask2_itk);

  // Write mask if needed
  if (mask_output_filename != "") {
    syd::WriteImage<itk::Image<unsigned char,3>>(itk_mask, mask_output_filename);
  }

  return stat;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiStatistic::pointer
syd::NewRoiStatistic(syd::Image::pointer image,
                     syd::RoiMaskImage::pointer mask,
                     std::string mask_output_filename)
{
  // new
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::RoiStatistic::pointer stat;
  db->New(stat);
  stat->image = image;
  stat->mask = mask;

  // copy the image tags
  syd::AddTag(stat->tags, image->tags);

  // update
  auto itk_mask = syd::UpdateRoiStatistic(stat);

  // Write mask if needed
  if (mask_output_filename != "") {
    syd::WriteImage<itk::Image<unsigned char,3>>(itk_mask, mask_output_filename);
  }

  return stat;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiStatistic::pointer
syd::NewRoiStatistic(syd::TiaImage::pointer tia,
                     syd::RoiMaskImage::pointer mask,
                     std::string mask_output_filename)
{
  auto stat = NewRoiStatistic(tia->GetOutput("fit_auc"), mask,
                              tia->GetOutput("fit_success"),
                              mask_output_filename);
  return stat;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
itk::Image<unsigned char, 3>::Pointer
syd::UpdateRoiStatistic(syd::RoiStatistic::pointer stat,
                        const itk::Image<unsigned char, 3>::Pointer itk_mask2)
{
  // Get the itk images
  typedef float PixelType; // whatever the image
  typedef unsigned char MaskPixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef itk::Image<MaskPixelType,3> MaskImageType;

  // read image (float)
  auto itk_input = syd::ReadImage<ImageType>(stat->image->GetAbsolutePath());

  // read mask (or create)
  MaskImageType::Pointer itk_mask;
  if (stat->mask == nullptr) {
    LOG(2) << "No mask (create temporary image).";
    itk_mask = syd::CreateImageLike<MaskImageType>(itk_input);
    itk_mask->FillBuffer(1);
  }
  else {
    itk_mask = syd::ReadImage<MaskImageType>(stat->mask->GetAbsolutePath());
    LOG(2) << "Input mask: " << stat->mask->GetAbsolutePath();
  }

  // Resampling. Should resample mask or image ???
  // I decide here to resample the mask.
  // Resample do nothing if the image sizes are equal
  if (!syd::ImagesHaveSameSupport<MaskImageType,ImageType>(itk_mask, itk_input)) {
    itk_mask = syd::ResampleAndCropImageLike<MaskImageType>(itk_mask, itk_input, 0, 0);
  }

  // Mask to combine
  if (itk_mask2 != nullptr) {
    auto itk_mask2_resampled = itk_mask2;
    if (!syd::ImagesHaveSameSupport<MaskImageType,MaskImageType>(itk_mask, itk_mask2))
      itk_mask2_resampled = syd::ResampleAndCropImageLike<MaskImageType>(itk_mask2, itk_mask, 0, 0);
    itk_mask = syd::AndImage<MaskImageType>(itk_mask, itk_mask2_resampled);
  }

  // Statistics
  typedef itk::LabelStatisticsImageFilter<ImageType, MaskImageType> FilterType;
  auto filter = FilterType::New();
  filter->SetInput(itk_input);
  filter->SetLabelInput(itk_mask);
  filter->Update();
  double mean = filter->GetMean(1);
  double std = filter->GetSigma(1);
  double n = filter->GetCount(1);
  double min = filter->GetMinimum(1);
  double max = filter->GetMaximum(1);
  double sum = filter->GetSum(1);

  // Set the statistic values
  stat->mean = mean;
  stat->std_dev = std;
  stat->n = n;
  stat->min = min;
  stat->max = max;
  stat->sum = sum;

  // return the used mask
  return itk_mask;
}
// --------------------------------------------------------------------


