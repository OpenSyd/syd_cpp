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
#include "sydImageFromDicomBuilder.h"
#include "sydImageUtils.h"

// --------------------------------------------------------------------
syd::Image::pointer syd::ImageFromDicomBuilder::NewImageFromDicom(const syd::DicomSerie::pointer dicom)
{
  syd::Image::pointer image = NewMHDImage(dicom);

  try {
    if (dicom->dicom_modality == "CT") {
      typedef short PixelType;
      typedef itk::Image<PixelType,3> ImageType;
      LOG(4) << "Read dicom (short)";
      ImageType::Pointer itk_image = db_->ReadImage<PixelType>(dicom, true);
      LOG(4) << "Update information";
      syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("HU", "Hounsfield Units");
      image->pixel_unit = unit;
      if (dicom->pixel_scale != 1.0) {
        LOG(2) << "Pixel Scale = " << dicom->pixel_scale << ", so I scale the image.";
        syd::ScaleImage<ImageType>(itk_image, dicom->pixel_scale);
      }
      SetImage<PixelType>(image, itk_image);
    }
    else {
      typedef float PixelType;
      typedef itk::Image<PixelType,3> ImageType;
      LOG(4) << "Read dicom (float)";
      ImageType::Pointer itk_image = db_->ReadImage<PixelType>(dicom, true);
      LOG(4) << "Update information";
      syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("counts", "Number of counts");
      image->pixel_unit = unit;
      // Scale if needed
      if (dicom->pixel_scale != 1.0) {
        LOG(2) << "Pixel Scale = " << dicom->pixel_scale << ", so I scale the image.";
        syd::ScaleImage<ImageType>(itk_image, dicom->pixel_scale);
      }
      SetImage<PixelType>(image, itk_image);
    }

  } catch (syd::Exception & e) {
    EXCEPTION("Error during InsertImage: " << e.what());
  }

  // return the image
  return image;
}
// --------------------------------------------------------------------
