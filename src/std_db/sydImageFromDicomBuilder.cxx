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
syd::ImageFromDicomBuilder::ImageFromDicomBuilder(syd::StandardDatabase * db)
{
  db_ = db;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::ImageFromDicomBuilder::CreateImageFromDicom(const syd::DicomSerie::pointer dicom)
{
  syd::Image::pointer image;
  db_->New(image);
  image->patient = dicom->patient;
  image->dicoms.push_back(dicom);
  image->frame_of_reference_uid = dicom->dicom_frame_of_reference_uid;
  image->type = "mhd";
  db_->Insert(image); // to obtain an id

  std::string mhd_path = image->ComputeDefaultAbsolutePath(db_);

  // Update the Files
  image->UpdateFile(db_, mhd_path);

  try {
    if (dicom->dicom_modality == "CT") {
      typedef short PixelType;
      typedef itk::Image<PixelType,3> ImageType;
      LOG(4) << "Read dicom (short)";
      ImageType::Pointer itk_image = db_->ReadImage<PixelType>(dicom, true);
      LOG(4) << "Update information";
      db_->UpdateImageInfo<PixelType>(image, itk_image, true, true); // true = update md5
      LOG(4) << "Write image on disk " << mhd_path;
      syd::WriteImage<ImageType>(itk_image, mhd_path);
      syd::Tag::pointer tag_ct = db_->FindOrInsertTag("ct", "CT scan image");
      image->AddTag(tag_ct);
      syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("HU", "Hounsfield Units");
      image->pixel_value_unit = unit;
    }
    else {
      // No auto tag, we dont know if this is a spect, a pet etc
      typedef float PixelType;
      typedef itk::Image<PixelType,3> ImageType;
      LOG(4) << "Read dicom (float)";
      ImageType::Pointer itk_image = db_->ReadImage<PixelType>(dicom, true);
      LOG(4) << "Update information";
      db_->UpdateImageInfo<PixelType>(image, itk_image, true, true); // true = update md5
      LOG(4) << "Write image on disk " << mhd_path;
      syd::WriteImage<ImageType>(itk_image, mhd_path);
      syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("counts", "Number of counts");
      image->pixel_value_unit = unit;
    }

  } catch (syd::Exception & e) {
    EXCEPTION("Error during InsertImage: " << e.what());
  }

  // Final update
  db_->Update(image);
  return image;
}
// --------------------------------------------------------------------
