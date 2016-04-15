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
syd::RoiMaskImage::pointer
syd::RoiMaskImageBuilder::NewRoiMaskImage(const syd::DicomSerie::pointer & dicom,
                                          const syd::RoiType::pointer & roitype,
                                          const std::string & filename)
{
  // First check if the file is ok
  fs::path dir(filename);
  if (!fs::exists(dir)) {
    EXCEPTION("Could not read the file '" << filename << "'.");
  }
  auto fake = syd::ReadImageHeader(filename); // -> will raise an exception if bug

  // Create image
  syd::RoiMaskImage::pointer mask = NewMHDRoiMaskImage(dicom->patient, roitype, dicom->injection);
  mask->dicoms.push_back(dicom);
  mask->frame_of_reference_uid = dicom->dicom_frame_of_reference_uid;
  syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("label", "Mask image");
  mask->pixel_value_unit = unit;
  CopyImageFromFile(mask, filename);
  return mask;
}
// --------------------------------------------------------------------
