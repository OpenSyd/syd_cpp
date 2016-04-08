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
#include "sydTimePointsBuilder.h"

// itk
#include <itkLabelStatisticsImageFilter.h>

// --------------------------------------------------------------------
syd::TimePointsBuilder::TimePointsBuilder(syd::StandardDatabase * db):
  syd::DatabaseFilter(db)
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimePointsBuilder::ComputeTimePoints(syd::TimePoints::pointer tac,
                                               const syd::Image::vector images,
                                               const syd::RoiMaskImage::pointer mask)
{
  DD("ComputeTimePoints");

  // image type
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef unsigned char MaskPixelType;
  typedef itk::Image<MaskPixelType,3> MaskImageType;

  // Sort images by date
  syd::Image::vector sorted_images = images;
  db_->Sort<syd::Image>(sorted_images);

  // Get the times
  std::vector<double> times = syd::GetTimesFromInjection(db_, sorted_images);
  DDS(times);

  // read all itk images
  std::vector<ImageType::Pointer> itk_images;
  for(auto image:sorted_images) {
    auto itk_image = syd::ReadImage<ImageType>(db_->GetAbsolutePath(image));
    itk_images.push_back(itk_image);
  }
  DD("read done");

  // declare itk mask
  MaskImageType::Pointer itk_mask;

  // Loop on every images
  std::vector<double> means;
  std::vector<double> stddevs;
  for(auto itk_image:itk_images) {
    // resample the mask as the first itk image
    if (!itk_mask or
        !syd::CheckImageSameSizeAndSpacing<ImageType::ImageDimension>(itk_mask, itk_image)) {
      DD("read and resample")
      itk_mask = syd::ReadImage<MaskImageType>(db_->GetAbsolutePath(mask));
      itk_mask = syd::ResampleAndCropImageLike<MaskImageType>(itk_mask, itk_image, 0, 0);
    }

    // Get mean value
    typedef itk::LabelStatisticsImageFilter<ImageType, MaskImageType> FilterType;
    typename FilterType::Pointer filter=FilterType::New();
    filter->SetInput(itk_image);
    filter->SetLabelInput(itk_mask);
    filter->Update();
    double mean = filter->GetMean(1);
    double stddev = filter->GetSigma(1);
    means.push_back(mean);
    stddevs.push_back(stddev);
  }
  DDS(means);
  DDS(stddevs);

  // Create tac
  auto n = times.size();
  tac->times.resize(n);
  tac->values.resize(n);
  tac->std_deviations.resize(n);
  for(auto i=0; i<n; i++) {
    tac->times[i] = times[i];
    tac->values[i] = means[i];
    tac->std_deviations[i] = stddevs[i];
  }
  DD(tac);

}
// --------------------------------------------------------------------
