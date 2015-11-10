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
#include "sydStitchDicomImageBuilder.h"
#include "sydImageUtils.h"

// --------------------------------------------------------------------
syd::Image::pointer
syd::StitchDicomImageBuilder::NewStitchedImage(const syd::DicomSerie::pointer a,
                                               const syd::DicomSerie::pointer b)
{
  // Check a and b modality
  if (a->dicom_modality != b->dicom_modality) {
    LOG(FATAL) << "Error while trying to stitch the two following images, dicom_modality is not the same: "
               << std::endl << a
               << std::endl << b;
  }

  // Check a and b patient
  if (a->patient->id != b->patient->id) {
    LOG(FATAL) << "Error while trying to stitch the two following images, patient is not the same: "
               << std::endl << a
               << std::endl << b;
  }

  // Only consider single file and not CT for the moment
  if (a->dicom_modality == "CT") {
    LOG(FATAL) << "Error cannot stitch CT images. Only float pixel type (yet).";
  }

  // Check if the same frame_of_reference_uid (flag to bypass ?)
  if (a->dicom_frame_of_reference_uid != b->dicom_frame_of_reference_uid) {
    LOG(FATAL) << "Error, cannot stitch those two dicoms because the frame_of_reference_uid is not the same:"
               << std::endl << a << " " << a->dicom_frame_of_reference_uid
               << std::endl << b << " " << b->dicom_frame_of_reference_uid;
  }

  // Create the image record
  syd::Image::pointer image = NewMHDImage(a);
  image->dicoms.push_back(b);
  syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("counts", "Number of counts");
  image->pixel_value_unit = unit;

  // Read the dicom images
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  ImageType::Pointer image_a = db_->ReadImage<PixelType>(a, true);
  ImageType::Pointer image_b = db_->ReadImage<PixelType>(b, true);

  // Stitch (default values for now, to be changed!)
  ImageType::Pointer output = syd::StitchImages<ImageType>(image_a, image_b, 150000, 4);
  SetImage<PixelType>(image, output);
  return image;
}
// --------------------------------------------------------------------
