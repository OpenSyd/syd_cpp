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
#include "sydRoiMaskImageHelper.h"
#include "sydTagHelper.h"
#include "sydImageCrop.h"
#include "sydImageAnd.h"

// itk
#include <itkLabelStatisticsImageFilter.h>

// --------------------------------------------------------------------
syd::RoiType::pointer syd::FindRoiType(const std::string & roiname,
                                       syd::StandardDatabase * db)
{
  try {
    syd::RoiType::pointer r;
    odb::query<syd::RoiType> q = odb::query<RoiType>::name == roiname;
    db->QueryOne(r, q);
    return r;
  } catch (const std::exception& e) {
    EXCEPTION("Cannot find a RoiType named '" << roiname << "'.");
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer
syd::FindRoiMaskImage(const syd::Image::pointer image,
                      const std::string & roi_name)
{
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::RoiType::pointer roitype = syd::FindRoiType(roi_name, db);
  try {
    syd::RoiMaskImage::pointer roi;
    odb::query<syd::RoiMaskImage> q =
      odb::query<syd::RoiMaskImage>::roitype == roitype->id and
      odb::query<syd::RoiMaskImage>::frame_of_reference_uid ==
      image->frame_of_reference_uid;
    db->QueryOne(roi, q);
    return roi;
  } catch (const std::exception& e) {
    EXCEPTION("Cannot find a RoiMaskImage for image " << image->id
              << " named '" << roi_name << "' (with same frame_of_reference_uid).");
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer
syd::InsertRoiMaskImageFromFile(std::string filename,
                                syd::Patient::pointer patient,
                                syd::RoiType::pointer roitype)
{
  auto db = patient->GetDatabase<syd::StandardDatabase>();
  syd::RoiMaskImage::pointer mask;
  db->New(mask);
  mask->patient = patient;
  mask->type = "mhd";
  mask->roitype = roitype;
  mask->modality = "mask"; // force to mask
  mask->pixel_unit = syd::FindPixelUnit(db, "label");
  db->Insert(mask);
  mask->files = syd::InsertFilesFromMhd(db, filename,
                                        mask->ComputeDefaultRelativePath(),
                                        mask->ComputeDefaultMHDFilename());
  syd::SetImageInfoFromFile(mask);
  db->Update(mask);
  return mask;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiStatistic::pointer syd::FindOneRoiStatistic(syd::Image::pointer image,
                                                    syd::RoiMaskImage::pointer mask)
{
  typedef odb::query<syd::RoiStatistic> Q;
  Q q = Q::image == image->id;
  if (mask != nullptr) q = q and (Q::mask == mask->id);
  syd::RoiStatistic::vector stats;
  auto db = image->GetDatabase<syd::StandardDatabase>();
  db->Query(stats, q);
  if (stats.size() == 0) {
    EXCEPTION("Cannot find RoiStatistic for this image and this mask: " << image
              << " " << (mask == nullptr ? "no_mask":mask->ToString()));
  }
  if (stats.size() == 1 and mask == nullptr) {
    if (stats[0]->mask != nullptr) {
      EXCEPTION("Cannot find RoiStatistic with this image and nomask " << image);
    }
  }
  if (stats.size() > 1) {
    int found = 0;
    if (mask == nullptr) { // if no mask search for all RoiStatistic
      for(auto s:stats) {
        if (s->mask == nullptr) {
          stats[0] = s;
          ++found; // count the number of RoiStatistic with no mask
        }
      }
    }
    if (found != 1) {
      EXCEPTION("Several RoiStatistic with image " << image->id
                << " and mask " << (mask != nullptr ? mask->id:0) << " ("
                << (mask != nullptr ? mask->roitype->name:"no_mask") << ")"
                << " exist. ");
    }
  }
  return stats[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiStatistic::pointer syd::InsertRoiStatistic(syd::Image::pointer image,
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

  // insert
  db->Insert(stat);

  // Write mask if needed
  if (mask_output_filename != "") {
    syd::WriteImage<itk::Image<unsigned char,3>>(itk_mask, mask_output_filename);
  }

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

  // read mask (or create
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


// --------------------------------------------------------------------
syd::RoiStatistic::pointer
syd::InsertOrUpdateRoiStatistic(syd::Image::pointer image,
                                std::string roi_name,
                                bool force_update)
{
  DDF();
  syd::RoiMaskImage::pointer mask;
  try {
    mask = syd::FindRoiMaskImage(image, roi_name);
  } catch(std::exception &) {
    //LOG(WARNING) << "InsertOrUpdate:: cannot find the roi " << roi_name;
    return nullptr;
  }
  DD(mask);
  // compute all stats for each inputs
  syd::RoiStatistic::pointer stat;
  try {
    stat = syd::FindOneRoiStatistic(image, mask);
    if (force_update) syd::UpdateRoiStatistic(stat);
  } catch(std::exception &) {
    stat = syd::InsertRoiStatistic(image, mask);
  }
  DD(stat);
  return stat;
}
// --------------------------------------------------------------------

