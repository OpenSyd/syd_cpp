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
#include "sydDecayCorrectedImageBuilder.h"

// --------------------------------------------------------------------
syd::DecayCorrectedImageBuilder::DecayCorrectedImageBuilder(StandardDatabase * db):DecayCorrectedImageBuilder()
{
  SetDatabase(db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DecayCorrectedImageBuilder::DecayCorrectedImageBuilder()
{
  // init
  db_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer
syd::DecayCorrectedImageBuilder::CreateDecayCorrectedImage(syd::Image::pointer input,
                                                           syd::Calibration::pointer calib)
{
  // Get information
  if (input->dicoms.size() < 1) {
    LOG(FATAL) << "Error this image is not associated with a dicom. ";
  }
  syd::DicomSerie::pointer dicom = input->dicoms[0];
  syd::Injection::pointer injection = dicom->injection;
  if (injection == NULL) {
    LOG(FATAL) << "Error this dicom is not associated with an injection :" << input->dicoms[0];
  }

  double injected_activity = injection->activity_in_MBq;
  double time = syd::DateDifferenceInHours(dicom->acquisition_date, injection->date);
  double lambda = log(2.0)/(injection->radionuclide->half_life_in_hours);

  // Create output image
  syd::Image::pointer result = syd::Image::New();
  result = input; // copy the fields
  result->id = -1; // but change the ID to insert as a new image.

  // FIXME --> change equation to take spect acquisition time into account (how to do when 2 spects ?)

  // pixel = value x calibration_factor / injected_MBq x exp(lambda x t)
  double f = calib->factor / injected_activity * exp(lambda * time);

  // Change pixel values
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  ImageType::Pointer itk_image = syd::ReadImage<ImageType>(db_->GetAbsolutePath(input));
  itk::ImageRegionIterator<ImageType> iter(itk_image, itk_image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd()) {
    iter.Set(iter.Get() / f);
    ++iter;
  }

  // Create and image file
  db_->Insert(result);
  std::string mhd_path = result->ComputeDefaultFilename(db_);
  syd::WriteImage<ImageType>(itk_image, mhd_path);
  result->UpdateFile(db_, mhd_path);

  db_->Update(result);
  return result;
}
// --------------------------------------------------------------------
