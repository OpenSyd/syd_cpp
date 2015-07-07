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
  image->type = "mhd";
  db_->Insert(image); // to obtain an id

  std::ostringstream oss;
  oss << dicom->dicom_modality << "_" << image->id << ".mhd";
  std::string mhd_filename = oss.str();
  std::string mhd_relative_path = image->ComputeRelativeFolder()+PATH_SEPARATOR;
  std::string mhd_path = db_->ConvertToAbsolutePath(mhd_relative_path+mhd_filename);

  // Update the Files
  image->UpdateFile(db_, mhd_filename, mhd_relative_path);

  try {
    if (dicom->dicom_modality == "CT") {
      typedef short PixelType;
      typedef itk::Image<PixelType,3> ImageType;
      LOG(4) << "Read dicom (short)";
      ImageType::Pointer itk_image = db_->ReadImage<PixelType>(dicom);
      UpdateImageInfo<PixelType>(image, itk_image, true); // true = update md5
      LOG(4) << "Write image on disk " << mhd_path;
      syd::WriteImage<ImageType>(itk_image, mhd_path);
    }
    else {
      // No auto tag, we dont know if this is a spect, a pet etc
      typedef float PixelType;
      typedef itk::Image<PixelType,3> ImageType;
      LOG(4) << "Read dicom (float)";
      ImageType::Pointer itk_image = db_->ReadImage<PixelType>(dicom);
      UpdateImageInfo<PixelType>(image, itk_image, true); // true = update md5
      LOG(4) << "Write image on disk " << mhd_path;
      syd::WriteImage<ImageType>(itk_image, mhd_path);
    }

  } catch (syd::Exception & e) {
    EXCEPTION("Error during InsertImage: " << e.what());
  }

  // Final update
  db_->Update(image);
  return image;
}
// --------------------------------------------------------------------
