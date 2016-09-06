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
#include "sydRoiStatisticBuilder.h"

// itk
#include <itkLabelStatisticsImageFilter.h>

// --------------------------------------------------------------------
syd::RoiStatisticBuilder::RoiStatisticBuilder(syd::StandardDatabase * db)
{
  SetDatabase(db);
  SetEmptyPixelValueFlag(false);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer
syd::RoiStatisticBuilder::FindMask(const syd::Image::pointer input,
                                   const std::string roiname)
{
  syd::RoiType::pointer roitype = syd::FindRoiType(roiname, db);
  typedef odb::query<syd::RoiMaskImage> Q;
  Q q = Q::roitype == roitype->id and Q::frame_of_reference_uid == input->frame_of_reference_uid;
  syd::RoiMaskImage::vector roimasks;
  db_->Query(roimasks, q);
  if (roimasks.size() == 0) {
    LOG(FATAL) << "Cannot find a roimask of type '" << roitype->name
               << "' with same frame_of_reference_uid for the image: " << input;
  }
  if (roimasks.size() > 1) {
    LOG(FATAL) << "Several roimask exist with type '" << roitype->name
               << "' and same frame_of_reference_uid for the image: " << input;
  }
  return roimasks[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::RoiStatisticBuilder::Exists(syd::RoiStatistic::pointer * stat,
                                      const syd::Image::pointer image,
                                      const syd::RoiMaskImage::pointer mask)
{
  typedef odb::query<syd::RoiStatistic> Q;
  Q q = Q::mask == mask->id and Q::image == image->id;
  try {
    db_->QueryOne(*stat, q);
    return true;
  } catch(std::exception & e) {
    return false;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RoiStatisticBuilder::ComputeStatistic(syd::RoiStatistic::pointer stat)
{
  // Get the itk images
  typedef float PixelType; // whatever the image
  typedef unsigned char MaskPixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef itk::Image<MaskPixelType,3> MaskImageType;
  ImageType::Pointer itk_input =
    syd::ReadImage<ImageType>(stat->image->GetAbsolutePath());
  MaskImageType::Pointer itk_mask;

  if (stat->mask == NULL) {
    LOG(2) << "No mask (create temporary image).";
    itk_mask = syd::CreateImageLike<MaskImageType>(itk_input);
    itk_mask->FillBuffer(1);
  }
  else {
    itk_mask = syd::ReadImage<MaskImageType>(stat->mask->GetAbsolutePath());
    LOG(2) << "Input mask: " << stat->mask->GetAbsolutePath();
  }


  // FIXME resampling. Resample mask or image ?
  // start by resampling the mask
  // If need resample input: consider total pixel counts.


  // Check same size / spacing
  if (!syd::CheckImageSameSizeAndSpacing<ImageType::ImageDimension>(itk_mask, itk_input))
    itk_mask = syd::ResampleAndCropImageLike<MaskImageType>(itk_mask, itk_input, 0, 0);

  // Add "empty" values to the mask
  if (useEmptyPixelValueFlag_) {
    // loop itk_input ; change itk_mask
    typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
    typedef itk::ImageRegionIterator<MaskImageType> IteratorType;
    ConstIteratorType iter1(itk_input,itk_input->GetLargestPossibleRegion());
    IteratorType iter2(itk_mask,itk_mask->GetLargestPossibleRegion());
    iter1.GoToBegin();
    iter2.GoToBegin();
    while (!iter1.IsAtEnd()) {
      typename ImageType::PixelType v1 = iter1.Get();
      typename ImageType::PixelType v2 = iter2.Get();
      if (v1 == emptyPixelValue_) iter2.Set(0);// 0 is background
      ++iter1; ++iter2;
    }
  }

  // Statistics
  typedef itk::LabelStatisticsImageFilter<ImageType, MaskImageType> FilterType;
  typename FilterType::Pointer filter=FilterType::New();
  filter->SetInput(itk_input);
  filter->SetLabelInput(itk_mask);
  filter->Update();
  double mean = filter->GetMean(1);
  double std = filter->GetSigma(1);
  double n = filter->GetCount(1);
  double min = filter->GetMinimum(1);
  double max = filter->GetMaximum(1);
  double sum = filter->GetSum(1);

  // Update value
  stat->mean = mean;
  stat->std_dev = std;
  stat->n = n;
  stat->min = min;
  stat->max = max;
  stat->sum = sum;
}
// --------------------------------------------------------------------
