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
void syd::TimePointsBuilder::SetImages(const syd::Image::vector im)
{
  images = im;
  db_->Sort<syd::Image>(images);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimePointsBuilder::SetRoiMaskImage(const syd::RoiMaskImage::pointer m)
{
  mask = m;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimePoints::pointer
syd::TimePointsBuilder::ComputeTimePoints()
{
  DD("ComputeTimePoints");

  // Check
  if (images.size() ==0) {
    EXCEPTION("Cannot compute TimePoints, no images. Use SetImages first");
  }
  if (!mask) {
    EXCEPTION("Cannot compute TimePoints, no mask. Use SetRoiMaskImage first");
  }

  // Check same injection date
  images[0]->FatalIfNoDicom();
  syd::Injection::pointer injection = images[0]->dicoms[0]->injection;
  bool b = true;
  for(auto image:images) {
    image->FatalIfNoDicom();
    b = b and (injection->id == image->dicoms[0]->injection->id);
  }
  if (!b) {
    EXCEPTION("The images do not have the same injection.");
  }

  // Check same pixel units (warning)
  syd::PixelValueUnit::pointer unit = images[0]->pixel_value_unit;
  for(auto image:images) {
    if (image->pixel_value_unit->id != unit->id) {
      LOG(WARNING) << "I expected pixel value unit to be the same for all images, while it is "
                   << image->pixel_value_unit->name << "for the image:"
                   << std::endl << image
                   << std::endl << " and " << unit->name << " for the image"
                   << std::endl << images[0];
    }
  }

  // Check if already exist ?
  syd::TimePoints::vector timepoints;
  syd::TimePoints::pointer tac;
  odb::query<syd::TimePoints> q = odb::query<syd::TimePoints>::mask == mask->id;
  db_->Query(timepoints,q);
  DDS(timepoints);
  auto n = images.size();
  int found = 0;
  for(auto t:timepoints) {
    if (t->images.size() == n) {
      bool b = true;
      for(auto i=0; i<n; i++) {
        if (t->images[i]->id != images[i]->id) b = false;
      }
      if (b) {
        ++found;
        tac = t; // found a similar timepoints !
      }
    }
  }
  DD(found);
  if (found == 0) {
    db_->New(tac);
    tac->images = images;
    tac->mask = mask;
  }
  if (found == 1) {
    DD(tac);
    // update ? check history ?
  }
  if (found > 1) {
    EXCEPTION("Several TimePoints found with the same set of images/mask. Abort");
  }

  // image type
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef unsigned char MaskPixelType;
  typedef itk::Image<MaskPixelType,3> MaskImageType;

  // Get the times
  std::vector<double> times = syd::GetTimesFromInjection(db_, images);
  DDS(times);

  // read all itk images
  std::vector<ImageType::Pointer> itk_images;
  for(auto image:images) {
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
  tac->times.resize(n);
  tac->values.resize(n);
  tac->std_deviations.resize(n);
  for(auto i=0; i<n; i++) {
    tac->times[i] = times[i];
    tac->values[i] = means[i];
    tac->std_deviations[i] = stddevs[i];
  }
  DD(tac);
  DD("done");
  return tac;
}
// --------------------------------------------------------------------
