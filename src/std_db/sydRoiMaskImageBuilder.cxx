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
#include "sydRoiMaskImageBuilder.h"

// --------------------------------------------------------------------
syd::RoiMaskImageBuilder::RoiMaskImageBuilder(syd::StandardDatabase * db):syd::RoiMaskImageBuilder()
{
  SetDatabase(db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImageBuilder::RoiMaskImageBuilder()
{
  db_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer
syd::RoiMaskImageBuilder::InsertRoiMaskImage(const syd::DicomSerie::pointer & dicom,
                                             const syd::RoiType::pointer & roitype,
                                             const std::string & filename)
{
  syd::RoiMaskImage::pointer mask;
  syd::Image::pointer image;
  try {
    // Create a mask
    db_->New(mask);
    mask->roitype = roitype;
    mask->patient = dicom->patient;
    mask->dicoms.push_back(dicom);
    mask->frame_of_reference_uid = dicom->dicom_frame_of_reference_uid;
    mask->type = "mhd";
    syd::Tag::pointer tag_mask = db_->FindOrInsertTag("mask", "Mask image");
    mask->AddTag(tag_mask);
    db_->Insert(mask);

    // Create filename
    std::ostringstream oss;
    oss << roitype->name << "_" << mask->id << ".mhd";
    std::string mhd_filename = oss.str();
    std::string mhd_relative_path = mask->ComputeRelativeFolder()+PATH_SEPARATOR;
    std::string mhd_path = db_->ConvertToAbsolutePath(mhd_relative_path+mhd_filename);

    // Update files in images
    mask->UpdateFile(db_, mhd_filename, mhd_relative_path);

    // Read image
    typedef unsigned char PixelType;
    typedef itk::Image<PixelType,3> ImageType;
    ImageType::Pointer itk_image;
    itk_image = syd::ReadImage<ImageType>(filename);

    // Update image info
    db_->UpdateImageInfo<PixelType>(mask, itk_image, true); // true = update md5
    LOG(4) << "Write image on disk " << mhd_path;
    syd::WriteImage<ImageType>(itk_image, mhd_path);


  } catch (syd::Exception & e) {
    EXCEPTION("Error during InsertImage: " << e.what());
  }

  // Final update
  db_->Update(mask);
  return mask;
}
// --------------------------------------------------------------------
