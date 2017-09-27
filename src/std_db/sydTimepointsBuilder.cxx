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
#include "sydTimepointsBuilder.h"

// itk
#include <itkLabelStatisticsImageFilter.h>

// --------------------------------------------------------------------
syd::TimepointsBuilder::TimepointsBuilder(syd::StandardDatabase * db):
  syd::DatabaseFilter(db)
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimepointsBuilder::SetImages(const syd::Image::vector im)
{
  images = im;
  db_->Sort<syd::Image>(images, "acquisition_date");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimepointsBuilder::SetRoiMaskImage(const syd::RoiMaskImage::pointer m)
{
  mask = m;
  pixel.clear();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimepointsBuilder::SetPixel(std::vector<double> & p)
{
  pixel = p;
  mask = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Timepoints::pointer
syd::TimepointsBuilder::ComputeTimepoints()
{
  // Check
  if (images.size() ==0) {
    EXCEPTION("Cannot compute Timepoints, no images. Use SetImages first");
  }

  if (!mask and pixel.size() ==0) {
    EXCEPTION("Cannot compute Timepoints, no mask or pixel. Use SetRoiMaskImage or SetPixel first");
  }

  // Check same injection date
  images[0]->FatalIfNoDicom();
  syd::Injection::pointer injection = images[0]->injection;
  bool b = true;
  for(auto image:images) {
    image->FatalIfNoDicom();
    b = b and (injection->id == image->injection->id);
  }
  if (!b) {
    EXCEPTION("The images do not have the same injection.");
  }

  // Check same pixel units (warning)
  syd::PixelValueUnit::pointer unit = images[0]->pixel_unit;
  for(auto image:images) {
    if (image->pixel_unit->id != unit->id) {
      LOG(WARNING) << "I expected pixel value unit to be the same for all images, while it is "
                   << image->pixel_unit->name << "for the image:"
                   << std::endl << image
                   << std::endl << " and " << unit->name << " for the image"
                   << std::endl << images[0];
    }
  }

  if (!mask) {
    return ComputeTimepointsForPixel();
  }

  // Check if already exist ? (same images)
  syd::Timepoints::vector timepoints;
  syd::Timepoints::pointer tac;
  odb::query<syd::Timepoints> q = odb::query<syd::Timepoints>::mask == mask->id;
  db_->Query(timepoints,q);
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
  if (found == 0) {
    db_->New(tac);
    tac->images = images;
    tac->mask = mask;
  }
  if (found > 1) {
    EXCEPTION("Several Timepoints found with the same set of images/mask. Abort");
  }

  // Get the times
  std::vector<double> times = syd::GetTimesFromInjection(db_, images);

  // read all itk images
  std::vector<ImageType::Pointer> itk_images;
  for(auto image:images) {
    auto itk_image = syd::ReadImage<ImageType>(db_->GetAbsolutePath(image));
    itk_images.push_back(itk_image);
  }

  // declare itk mask
  MaskImageType::Pointer itk_mask;

  // Loop on every images
  std::vector<double> means;
  std::vector<double> stddevs;
  for(auto itk_image:itk_images) {
    // resample the mask as the first itk image
    if (!itk_mask or
        !syd::CheckImageSameSizeAndSpacing<ImageType::ImageDimension>(itk_mask, itk_image)) {
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

  // Set tac
  tac->times.resize(n);
  tac->values.resize(n);
  tac->std_deviations.resize(n);
  for(auto i=0; i<n; i++) {
    tac->times[i] = times[i];
    tac->values[i] = means[i];
    tac->std_deviations[i] = stddevs[i];
  }
  tac->patient = images[0]->patient;
  tac->injection = images[0]->injection;
  return tac;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimepointsBuilder::SetFromModel(syd::Timepoints::pointer timepoints,
                                          const std::vector<double> & times,
                                          const syd::FitModelBase * model)
{
  timepoints->times.clear();
  timepoints->values.clear();
  timepoints->std_deviations.clear();
  for(auto i=0; i<times.size(); i++) {
    timepoints->times.push_back(times[i]);
    timepoints->values.push_back(model->GetValue(times[i]));
    timepoints->std_deviations.push_back(0.0);
  }
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
syd::Timepoints::pointer
syd::TimepointsBuilder::ComputeTimepointsForPixel()
{
  DDS(pixel);

  // Create tac
  syd::Timepoints::pointer tac;
  db_->New(tac);
  tac->images = images;
  tac->mask = NULL;

  // Get the times
  std::vector<double> times = syd::GetTimesFromInjection(db_, images);

  // read all itk images
  std::vector<ImageType::Pointer> itk_images;
  for(auto image:images) {
    auto itk_image = syd::ReadImage<ImageType>(db_->GetAbsolutePath(image));
    itk_images.push_back(itk_image);
  }

  // Get pixel index
  ImageType::IndexType index;
  ImageType::PointType point;
  point[0] = pixel[0];
  point[1] = pixel[1];
  point[2] = pixel[2];
  itk_images[0]->TransformPhysicalPointToIndex(point, index);
  DD(index);

  // Set tac
  auto n = itk_images.size();
  tac->times.resize(n);
  tac->values.resize(n);
  tac->std_deviations.resize(n);
  for(auto i=0; i<n; i++) {
    tac->times[i] = times[i];
    tac->values[i] = itk_images[i]->GetPixel(index);
    tac->std_deviations[i] = 0.0;
  }
  tac->patient = images[0]->patient;
  tac->injection = images[0]->injection;
  return tac;
}
// --------------------------------------------------------------------
