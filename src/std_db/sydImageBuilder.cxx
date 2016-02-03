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
#include "sydImageBuilder.h"
#include "sydFileBuilder.h"

// --------------------------------------------------------------------
void syd::ImageBuilder::InitializeMHDFiles(syd::Image::pointer image)
{
  image->type = "mhd";
  syd::FileBuilder fb(db_);
  syd::File::pointer file_mhd = fb.NewFile(".mhd");
  syd::File::pointer file_raw = fb.NewFile();
  std::string f = file_mhd->filename;
  syd::Replace(f, ".mhd", ".raw");
  fb.RenameFile(file_raw, file_raw->path, f);
  image->files.push_back(file_mhd);
  image->files.push_back(file_raw);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::ImageBuilder::NewMHDImageLike(syd::Image::pointer input)
{
  syd::Image::pointer image = NewMHDImage(input->patient);
  if (input->dicoms.size() != 0) {
    image->CopyDicomSeries(input);
    image->frame_of_reference_uid = input->frame_of_reference_uid;
  }
  image->CopyTags(input);
  image->pixel_value_unit = input->pixel_value_unit;
  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::ImageBuilder::NewMHDImage(syd::Patient::pointer patient)
{
  syd::Image::pointer image;
  db_->New(image);
  image->patient = patient;
  InitializeMHDFiles(image);
  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::ImageBuilder::NewMHDImage(syd::DicomSerie::pointer dicom)
{
  syd::Image::pointer image = NewMHDImage(dicom->patient);
  image->dicoms.push_back(dicom);
  image->frame_of_reference_uid = dicom->dicom_frame_of_reference_uid;
  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer syd::ImageBuilder::NewMHDRoiMaskImage(syd::Patient::pointer patient,
                                                                 syd::RoiType::pointer roitype)
{
  syd::RoiMaskImage::pointer mask;
  db_->New(mask);
  mask->patient = patient;
  mask->roitype = roitype;
  InitializeMHDFiles(mask);
  return mask;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilder::CopyImageFromFile(syd::Image::pointer image, std::string filename)
{
  if (!fs::exists(filename)) {
    EXCEPTION("The file '" << filename << "' cannot be found.");
  }

  // Read itk image header to get the pixel type
  auto header = syd::ReadImageHeader(filename);
  switch (header->GetComponentType()) {
  case itk::ImageIOBase::UCHAR:
    {
      typedef unsigned char PixelType;
      typedef itk::Image<PixelType, 3> ImageType;
      ImageType::Pointer itk_image = syd::ReadImage<ImageType>(filename);
      SetImage<PixelType>(image, itk_image);
      break;
    }
  case itk::ImageIOBase::SHORT:
    {
      typedef short PixelType;
      typedef itk::Image<PixelType, 3> ImageType;
      ImageType::Pointer itk_image = syd::ReadImage<ImageType>(filename);
      SetImage<PixelType>(image, itk_image);
      break;
    }
  default:
    {
      typedef float PixelType;
      typedef itk::Image<PixelType, 3> ImageType;
      ImageType::Pointer itk_image = syd::ReadImage<ImageType>(filename);
      SetImage<PixelType>(image, itk_image);
      break;
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilder::RenameToDefaultFilename(syd::Image::pointer image)
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

  // Rename the files
  syd::FileBuilder fb(db_);
  fb.RenameMHDFile(image->files[0], image->files[1], mhd_relative_path, mhd_filename);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilder::CheckMHDImage(syd::Image::pointer image)
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
void syd::ImageBuilder::RenameToDefaultFilename(syd::RoiMaskImage::pointer mask)
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
  syd::FileBuilder fb(db_);
  fb.RenameMHDFile(mask->files[0], mask->files[1], mhd_relative_path, mhd_filename);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilder::InsertAndRename(syd::Image::pointer image)
{
  db_->Insert(image);
  RenameToDefaultFilename(image);
  db_->Update(image);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilder::InsertAndRename(syd::RoiMaskImage::pointer mask)
{
  db_->Insert(mask);
  RenameToDefaultFilename(mask);
  db_->Update(mask);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilder::SetImagePixelValueUnit(syd::Image::pointer image, std::string pixel_unit)
{
  syd::PixelValueUnit::pointer unit;
  odb::query<syd::PixelValueUnit> q = odb::query<syd::PixelValueUnit>::name == pixel_unit;
  try {
    db_->QueryOne(unit, q);
    image->pixel_value_unit = unit;
  } catch(std::exception & e) {
    EXCEPTION("Cannot find the unit '" << pixel_unit << "'.");
  }
}
// --------------------------------------------------------------------
