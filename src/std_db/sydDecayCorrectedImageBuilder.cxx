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
syd::Image::pointer
syd::DecayCorrectedImageBuilder::NewDecayCorrectedImage(syd::Image::pointer input,
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
  syd::Image::pointer result = NewMHDImage(input->dicoms[0]);
  // result = input; // copy the fields
  // result->id = -1; // but change the ID to insert as a new image.
  // syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("MBq_by_IA", "Activity in MBq by injected activity in MBq");
  // syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("kBq_by_IA", "Activity in kBq by injected activity in MBq");
  syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("Bq_by_IA", "Activity in Bq by injected activity in MBq");
  result->pixel_value_unit = unit;
  result->pixel_type = input->pixel_type;
  result->CopyTags(input);
  result->CopyDicomSeries(input);

  // FIXME --> change equation to take spect acquisition time into account (how to do when 2 spects ?)

  // pixel = value / calibration_factor / injected_MBq x exp(lambda x t)
  double f = 1.0/calib->factor / injected_activity * exp(lambda * time)*1000*1000; // x1000 for MBq to kBq x1000 to Bq

  // Change pixel values
  typedef float PixelType; // (force 'float' pixel type)
  typedef itk::Image<PixelType,3> ImageType;
  ImageType::Pointer itk_image = syd::ReadImage<ImageType>(db_->GetAbsolutePath(input));
  itk::ImageRegionIterator<ImageType> iter(itk_image, itk_image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd()) {
    iter.Set(iter.Get() * f);
    ++iter;
  }

  // Set the image
  SetImage<PixelType>(result, itk_image);
  return result;
}
// --------------------------------------------------------------------
