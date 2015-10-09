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
#include "sydSubstituteRadionuclideImageBuilder.h"

// --------------------------------------------------------------------
syd::SubstituteRadionuclideImageBuilder::SubstituteRadionuclideImageBuilder(StandardDatabase * db):SubstituteRadionuclideImageBuilder()
{
  SetDatabase(db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::SubstituteRadionuclideImageBuilder::SubstituteRadionuclideImageBuilder()
{
  // init
  db_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer
syd::SubstituteRadionuclideImageBuilder::CreateRadionuclideSubstitutedImage(syd::Image::pointer input,
                                                                            syd::Radionuclide::pointer rad)
{
  // Input image MUST be decay corrected and in MBq by injected MBq units
  if (input->pixel_value_unit->name != "MBq_by_IA") {
    LOG(WARNING) << "The pixel value unit of the following image is not MBq_by_IA. Maybe an error ?"
                 << std::endl
                 << input;
  }

  // Get information
  if (input->dicoms.size() < 1) {
    LOG(FATAL) << "Error this image is not associated with a dicom. ";
  }
  syd::DicomSerie::pointer dicom = input->dicoms[0];
  syd::Injection::pointer injection = dicom->injection;
  if (injection == NULL) {
    LOG(FATAL) << "Error this dicom is not associated with an injection :" << input->dicoms[0];
  }

  double time = syd::DateDifferenceInHours(dicom->acquisition_date, injection->date);
  double lambda = log(2.0)/(rad->half_life_in_hours);

  // Create output image
  syd::Image::pointer result = syd::Image::New();
  result = input; // copy the fields
  result->id = -1; // but change the ID to insert as a new image.
  syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("MBq_by_IA", "Activity in MBq by injected activity in MBq");
  result->pixel_value_unit = unit;

  // Change pixel values
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  ImageType::Pointer itk_image = syd::ReadImage<ImageType>(db_->GetAbsolutePath(input));
  itk::ImageRegionIterator<ImageType> iter(itk_image, itk_image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd()) {
    iter.Set(iter.Get() / exp(-lambda * time));
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
