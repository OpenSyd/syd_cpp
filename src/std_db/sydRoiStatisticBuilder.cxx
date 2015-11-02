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
  input = NULL;
  output = NULL;
  mask = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiStatisticBuilder::RoiStatisticBuilder()
{
  db_ = NULL;
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::RoiStatisticBuilder::SetImage(syd::Image::pointer im)
{
  input = im;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RoiStatisticBuilder::SetRoiMaskImage(syd::RoiMaskImage::pointer m)
{
  mask = m;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RoiStatisticBuilder::SetRoiType(std::string roiname)
{
  if (input == NULL) {
    LOG(FATAL) << "Use SetImage before SetRoiType";
  }

  syd::RoiMaskImage::pointer roimask;
  syd::RoiType::pointer roitype = db_->FindRoiType(roiname);
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
  SetRoiMaskImage(roimasks[0]);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiStatistic::pointer
syd::RoiStatisticBuilder::ComputeStatistic()
{
  if (input == NULL) {
    LOG(FATAL) << "Use SetImage before ComputeStatistic";
  }

  // if (mask == NULL) { // No mask, create a temporary one
  //   LOG(FATAL) << "Use SetRoiMaskImage before ComputeStatistic";
  // }

  // log
  LOG(2) << "Input file: " << db_->GetAbsolutePath(input);

  // Get the itk images
  typedef float PixelType; // whatever the image
  typedef uchar MaskPixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef itk::Image<MaskPixelType,3> MaskImageType;
  ImageType::Pointer itk_input = syd::ReadImage<ImageType>(db_->GetAbsolutePath(input));
  MaskImageType::Pointer itk_mask;

  if (mask == NULL) {
    LOG(2) << "No mask (create temporary image).";
    itk_mask = syd::CreateImageLike<MaskImageType>(itk_input);
    itk_mask->FillBuffer(1);
  }
  else {
    itk_mask = syd::ReadImage<MaskImageType>(db_->GetAbsolutePath(mask));
    LOG(2) << "Input mask: " << db_->GetAbsolutePath(mask);
  }

  // FIXME resampling. Resample mask or image ?
  // start by resampling the mask
  // If need resample input: consider total pixel counts.

  // Check same size / spacing
  if (!syd::CheckImageSameSizeAndSpacing<ImageType::ImageDimension>(itk_mask, itk_input))
    itk_mask = syd::ResampleAndCropImageLike<MaskImageType>(itk_mask, itk_input, 0, 0);

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
  DD(sum);
  DD(n);

  // New object
  syd::RoiStatistic::pointer stat;
  db_->New(stat);
  stat->image = input;
  stat->mask = mask;
  stat->mean = mean;
  stat->std_dev = std;
  stat->n = n;
  stat->min = min;
  stat->max = max;
  stat->sum = sum;

  // Warning stat is not in the db
  return stat;
}
// --------------------------------------------------------------------
