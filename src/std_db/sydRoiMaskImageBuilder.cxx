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
syd::RoiMaskImageBuilder::RoiMaskImageBuilder(syd::StandardDatabase * db)
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
  db_->New(mask);
  mask->roitype = roitype;
  mask->patient = dicom->patient;
  mask->dicoms.push_back(dicom);
  mask->frame_of_reference_uid = dicom->dicom_frame_of_reference_uid;
  mask->type = "mhd";
  syd::Tag::pointer tag_mask = db_->FindOrInsertTag("mask", "Mask image");
  mask->AddTag(tag_mask);
  syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("label", "Mask image");
  mask->pixel_value_unit = unit;
  db_->Insert(mask);

  // Create filename
  std::string mhd_path = mask->ComputeDefaultAbsolutePath(db_);

  // Copy file
  syd::CopyMHDImage(filename, mhd_path);

  // Update image info
  mask->UpdateFile(db_, mhd_path, true);
  db_->UpdateImageInfoFromFile(mask, mhd_path, true, true);

  // Update db
  db_->Update(mask);
  return mask;
}
// --------------------------------------------------------------------
