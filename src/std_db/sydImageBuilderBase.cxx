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
#include "sydImageBuilderBase.h"

// --------------------------------------------------------------------
void syd::ImageBuilderBase::InitializeMHDFiles(syd::Image::pointer image)
{
  image->type = "mhd";
  syd::File::pointer file_mhd;
  db_->New(file_mhd);
  syd::File::pointer file_raw;
  db_->New(file_raw);
  image->files.push_back(file_mhd);
  image->files.push_back(file_raw);
  db_->Insert(image->files);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::ImageBuilderBase::InsertNewMHDImageLike(syd::Image::pointer input)
{
  syd::Image::pointer image = InsertNewMHDImage(input->patient);
  if (input->dicoms.size() != 0) {
    image->CopyDicomSeries(input);
    image->frame_of_reference_uid = input->frame_of_reference_uid;
  }
  image->CopyTags(input);
  image->pixel_value_unit = input->pixel_value_unit;
  RenameToDefaultFilename(image);
  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::ImageBuilderBase::InsertNewMHDImage(syd::Patient::pointer patient)
{
  syd::Image::pointer image;
  db_->New(image);
  image->patient = patient;
  InitializeMHDFiles(image);
  db_->Insert(image);
  RenameToDefaultFilename(image);
  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::ImageBuilderBase::InsertNewMHDImage(syd::DicomSerie::pointer dicom)
{
  syd::Image::pointer image = InsertNewMHDImage(dicom->patient);
  image->dicoms.push_back(dicom);
  image->frame_of_reference_uid = dicom->dicom_frame_of_reference_uid;
  RenameToDefaultFilename(image);
  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer syd::ImageBuilderBase::InsertNewMHDRoiMaskImage(syd::Patient::pointer patient,
                                                                           syd::RoiType::pointer roitype)
{
  syd::RoiMaskImage::pointer mask;
  db_->New(mask);
  mask->patient = patient;
  mask->roitype = roitype;
  db_->New(mask);
  InitializeMHDFiles(mask);
  db_->Insert(mask);
  RenameToDefaultFilename(mask);
  return mask;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilderBase::UpdateImageFromFile(syd::Image::pointer image, std::string filename)
{
  // Read itk image header to get the pixel type
  auto header = syd::ReadImageHeader(filename);
  switch (header->GetComponentType()) {
  case itk::ImageIOBase::UCHAR:
    {
      typedef unsigned char PixelType;
      typedef itk::Image<PixelType, 3> ImageType;
      ImageType::Pointer itk_image = syd::ReadImage<ImageType>(filename);
      UpdateImage<PixelType>(image, itk_image);
      break;
    }
  case itk::ImageIOBase::SHORT:
    {
      typedef short PixelType;
      typedef itk::Image<PixelType, 3> ImageType;
      ImageType::Pointer itk_image = syd::ReadImage<ImageType>(filename);
      UpdateImage<PixelType>(image, itk_image);
      break;
    }
  default:
    {
      typedef float PixelType;
      typedef itk::Image<PixelType, 3> ImageType;
      ImageType::Pointer itk_image = syd::ReadImage<ImageType>(filename);
      UpdateImage<PixelType>(image, itk_image);
      break;
    }
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilderBase::RenameToDefaultFilename(syd::Image::pointer image)
{
  // Check
  CheckMHDImage(image);

  // Compute the default filename
  std::ostringstream oss;
  if (image->dicoms.size() != 0) oss << image->dicoms[0]->dicom_modality;
  else  oss << "image";
  oss << "_" << image->id << ".mhd";
  std::string mhd_filename = oss.str();
  std::string raw_filename = mhd_filename;
  syd::Replace(raw_filename, ".mhd", ".raw");
  std::string mhd_relative_path = image->ComputeRelativeFolder()+PATH_SEPARATOR;
  std::string mhd_path = db_->ConvertToAbsolutePath(mhd_relative_path+mhd_filename);

  // Set the files
  db_->RenameFile(image->files[0], mhd_relative_path, mhd_filename);
  db_->RenameFile(image->files[1], mhd_relative_path, raw_filename);

  // Update
  db_->Update(image);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilderBase::CheckMHDImage(syd::Image::pointer image)
{
  if (image->type != "mhd") {
    EXCEPTION("I only know 'mhd' as image type, while it is: " << image->type);
  }

  // Check files
  if (image->files.size() != 2) {
    EXCEPTION("Error while updating the image, I expect 2 Files: " << image);
  }

  // Check the image is in the db
  if (image->id == -1) {
    EXCEPTION("Could not compute a default filename for this image, it is not persistant: " << image);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilderBase::RenameToDefaultFilename(syd::RoiMaskImage::pointer mask)
{
  // Check
  CheckMHDImage(mask);

  // Compute the default filename
  std::ostringstream oss;
  if (mask->roitype == NULL) oss << "roi";
  else oss << mask->roitype->name;
  oss << "_" << mask->id << ".mhd";
  std::string mhd_filename = oss.str();
  std::string raw_filename = mhd_filename;
  syd::Replace(raw_filename, ".mhd", ".raw");
  std::string mhd_relative_path = mask->ComputeRelativeFolder()+PATH_SEPARATOR;
  std::string mhd_path = db_->ConvertToAbsolutePath(mhd_relative_path+mhd_filename);

  // Set the files
  db_->RenameFile(mask->files[0], mhd_relative_path, mhd_filename);
  db_->RenameFile(mask->files[1], mhd_relative_path, raw_filename);

  // Update
  db_->Update(mask);
}
// --------------------------------------------------------------------
