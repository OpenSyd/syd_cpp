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
#include "sydInsertTimepointCommand.h"
#include "sydImage.h"

// --------------------------------------------------------------------
syd::InsertTimepointCommand::InsertTimepointCommand(std::string db):DatabaseCommand()
{
  sdb_ = syd::Database::OpenDatabaseType<StudyDatabase>(db);
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertTimepointCommand::InsertTimepointCommand(syd::StudyDatabase  * db):
  DatabaseCommand(), sdb_(db)
{
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimepointCommand::Initialization()
{
  cdb_ = sdb_->get_clinical_database();
  ct_selection_patterns_.clear();
  set_ignore_files_flag(false);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertTimepointCommand::~InsertTimepointCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimepointCommand::set_ct_selection_patterns(std::string s)
{
  std::istringstream iss(s);
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(ct_selection_patterns_));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimepointCommand::InsertTimepoint(std::vector<std::string> inputs)
{
  // Convert serie to u long
  std::vector<IdType> ids;
  for(auto i=0; i<inputs.size(); i++) {
    IdType id = toULong(inputs[i]);
    ids.push_back(id);
  }

  // Insert all series
  for(auto i: ids) {
    Serie serie = cdb_->GetById<Serie>(i);
    InsertTimepoint(serie);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimepointCommand::InsertTimepoint(const Serie & serie)
{
  // Check modality
  if (serie.modality != "NM") {
    LOG(FATAL) << "Error the serie " << serie.id << " modality is " << serie.modality
               << " while expecting NM";
  }

  // Get patient
  Patient patient;
  if (cdb_->GetIfExist<Patient>(odb::query<Patient>::id == serie.patient_id, patient)) {
    cdb_->CheckPatient(patient);
  }
  else {
    LOG(FATAL) << "Error, the patient for this serie (" << serie << ") does not exist";
  }

  // Create a new Timepoint or get the existing one
  Timepoint timepoint;
  bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::spect_serie_id == serie.id, timepoint);

  // Get associated ct serie
  Serie ct_serie;
  cdb_->GetAssociatedCTSerie(serie.id, ct_selection_patterns_, ct_serie);

  // Get or create
  if (!b) {  // It does not exist, we create it
    VLOG(1) << "Creating new Timepoint for " << patient.name << " date " << serie.acquisition_date;
    RawImage spect;
    RawImage ct;
    timepoint.patient_id = patient.id;
    timepoint.number=0;
    sdb_->InsertTimepoint(timepoint, spect, ct);
    sdb_->UpdatePathAndRename(timepoint, false); // do not rename
  }
  else {
    if (!get_ignore_files_flag()) {
      VLOG(1) << "Timepoint " << patient.name << " "
              << timepoint.number << " "
              << timepoint.id << " "
              << serie.acquisition_date << " ("
              << timepoint.time_from_injection_in_hours
              << " hours) already exist, deleting current image and updating.";
      syd::DeleteMHDImage(sdb_->GetImagePath(timepoint.ct_image_id));
      syd::DeleteMHDImage(sdb_->GetImagePath(timepoint.spect_image_id));
    }
    else {
      VLOG(1) << "Timepoint " << patient.name << " "
              << timepoint.number << " "
              << timepoint.id << " "
              << serie.acquisition_date << " ("
              << timepoint.time_from_injection_in_hours
              << " hours) already exist, updating (without copying dicom).";
    }
  }

  // Update timepoint information
  timepoint.patient_id = patient.id;
  timepoint.spect_serie_id = serie.id;
  timepoint.ct_serie_id = ct_serie.id;
  timepoint.time_from_injection_in_hours = syd::DateDifferenceInHours(serie.acquisition_date, patient.injection_date);
  RawImage spect = sdb_->GetById<RawImage>(timepoint.spect_image_id);
  spect.pixel_type = "float";
  spect.path = patient.name+PATH_SEPARATOR;
  RawImage ct = sdb_->GetById<RawImage>(timepoint.ct_image_id);
  ct.pixel_type = "short";
  ct.path = patient.name+PATH_SEPARATOR;
  sdb_->Update(spect);
  sdb_->Update(ct);
  sdb_->Update(timepoint);

  // Convert dicom to image
  if (!get_ignore_files_flag()) {
    sdb_->ConvertDicomToImage(timepoint);
  }
  else { // ignore dicom copy
    if (get_update_md5_flag()) {
      sdb_->UpdateMD5(spect);
      sdb_->UpdateMD5(ct);
    }
  }

  // Update the filenames according to (compute number, time_from_injection_in_hours etc)
  sdb_->UpdateNumberAndRenameFiles(timepoint.patient_id);
}
// --------------------------------------------------------------------
