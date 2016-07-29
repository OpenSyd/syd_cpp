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
#include "sydPixelUnitHelper.h"

// --------------------------------------------------------------------
syd::ImageFromDicomBuilder::ImageFromDicomBuilder()
{
  dicom_ = NULL;
  image_ = NULL;
  user_pixel_type_ = "auto";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageFromDicomBuilder::SetInputDicomSerie(syd::DicomSerie::pointer dicom,
                                                    std::string pixel_type)
{
  dicom_ = dicom;
  user_pixel_type_ = pixel_type;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageFromDicomBuilder::Update()
{
  if (dicom_ == NULL) {
    EXCEPTION("Use SetInputDicomSerie first.");
  }
  auto db = dicom_->GetDatabase<syd::StandardDatabase>();
  db->New(image_);
  image_->patient = dicom_->patient;
  image_->modality = dicom_->dicom_modality; // needed to set the filename
  image_->acquisition_date = dicom_->dicom_acquisition_date;
  image_->frame_of_reference_uid = dicom_->dicom_frame_of_reference_uid;
  image_->dicoms.push_back(dicom_);

  // Get dicom associated files or folder
  dicom_files_ = dicom_->dicom_files;
  // odb::query<syd::DicomFile> q =
  //   odb::query<syd::DicomFile>::dicom_serie->id == dicom_->id;
  // db->Query(dicom_files_, q);
  if (dicom_files_.size() == 0) {
    EXCEPTION("Error not DicomFile associated with this DicomSerie: " << dicom_);
  }
  std::vector<std::string> dicom_filenames;
  for(auto f:dicom_files_)
    dicom_filenames.push_back(f->GetAbsolutePath());

  // read dicom header with itk header (first file only)
  header_ = syd::ReadImageHeader(dicom_files_[0]->GetAbsolutePath());
  std::string pixel_type =
    itk::ImageIOBase::GetComponentTypeAsString(header_->GetComponentType());

  // Switch pixel type if user ask
  if (user_pixel_type_ != "auto") {
    pixel_type = user_pixel_type_;
  }

  // Generate a temporary filename
  std::string t = db->GetDatabaseAbsoluteFolder()+
    PATH_SEPARATOR+"syd_temp_%%%%_%%%%_%%%%_%%%%.mhd";
  fs::path p = fs::unique_path(t);
  std::string temp_filename = p.string();

  // switch according to pixel type short, float other
  if (pixel_type == "short") WriteMhd<short>(temp_filename);
  else {
    if (pixel_type == "double") WriteMhd<double>(temp_filename);
    else {
      if (pixel_type == "unsigned_char") WriteMhd<unsigned char>(temp_filename);
      else {
        if (pixel_type == "float") WriteMhd<float>(temp_filename);
        else {
          LOG(FATAL) << "Pixel type '" << pixel_type
                     << "' (purposely) not implemented."
                     << "Try with option --pixel_type.";
        }
      }
    }
  }

  // Insert image in the db
  db->Insert(image_);
  // true = move instead of copy
  syd::ImageHelper::InsertMhdFiles(image_, temp_filename, true);

  // try to guess pixel_unit ?
  syd::PixelValueUnit::pointer unit = NULL;
  if (dicom_->dicom_modality == "CT") {
    try {
      unit = syd::PixelUnitHelper::FindPixelUnit(db, "HU");
    } catch(...) {} // ignore if not found
  }
  else {
    try {
      unit = syd::PixelUnitHelper::FindPixelUnit(db, "counts");
    } catch(...) {} // ignore if not found
  }
  image_->pixel_unit = unit;
}
// --------------------------------------------------------------------
