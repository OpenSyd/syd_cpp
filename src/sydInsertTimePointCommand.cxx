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
#include "sydInsertTimePointCommand.h"
#include "sydImage.h"

// --------------------------------------------------------------------
syd::InsertTimePointCommand::InsertTimePointCommand():DatabaseCommand()
{
  db_ = NULL;
  tpdb_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertTimePointCommand::~InsertTimePointCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimePointCommand::AddDatabase(syd::Database * d)
{
  if (databases_.size() == 0) { // first db
    DatabaseCommand::AddDatabase(d);
    tpdb_ = static_cast<TimePointsDatabase*>(d);
  }
  else {
    if (databases_.size() == 1) { // second db
      DatabaseCommand::AddDatabase(d);
      db_ = static_cast<ClinicalTrialDatabase*>(d);
    }
    else {
      LOG(FATAL) << "TimePointsDatabase::AddDatabase error. First provide TimePointsDatabase then ClinicalTrialDatabase.";
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimePointCommand::SetArgs(char ** inputs, int n)
{
  if (n < 2) {
    LOG(FATAL) << "At least two parameters are needed : <patient> <series ids>, but you provide "
               << n << " parameter(s)";
  }
  patient_name_ = inputs[0];
  for(auto i=1; i<n; i++) {
    IdType id = toULong(inputs[i]);
    serie_ids_.push_back(id);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimePointCommand::Run()
{
  // Check database
  if (db_ == NULL) {
    LOG(FATAL) << "A ClinicalTrialDatabase is needed in InsertTimePointCommand. Aborting.";
  }

  if (tpdb_ == NULL) {
    LOG(FATAL) << "A TimePointsDatabase is needed in InsertTimePointCommand. Aborting.";
  }

  // Set DB pointer
  tpdb_->set_clinicaltrial_database(db_);

  if (db_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name_, patient_)) {
    db_->CheckPatient(patient_);
  }
  else {
    LOG(FATAL) << "Error, the patient " << patient_name_ << " does not exist";
  }

  // Create folder if does not exist
  std::string path = tpdb_->GetFullPath(patient_);
  if (!OFStandard::dirExists(path.c_str())) {
    LOG(FATAL) << "The directory " << path << " does not exist. Please create.";
  }

  // Insert all series
  for(auto i=serie_ids_.begin(); i<serie_ids_.end(); i++)  {
    Serie serie = db_->GetById<Serie>(*i);
    Run(serie);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimePointCommand::Run(Serie serie)
{
  // Check modality
  if (serie.modality != "NM") {
    LOG(FATAL) << "Error the serie " << serie.id << " modality is " << serie.modality
               << " while expecting NM";
  }

  // Create or Update a new TimePoint
  TimePoint timepoint;
  bool b = tpdb_->GetIfExist<TimePoint>(odb::query<TimePoint>::serie_id == serie.id, timepoint);
  if (!b) {  // It does not exist, we create it
    VLOG(1) << "Creating new TimePoint";
    timepoint.patient_id = patient_.id;
    timepoint.serie_id = serie.id;
    timepoint.number=0;
    tpdb_->Insert(timepoint);
  }
  else {
    VLOG(1) << "TimePoint " << timepoint.number << " ("
            << timepoint.time_from_injection_in_hours
            << " hours) already exist, deleting current image and updating.";
    std::string path = tpdb_->GetFullPathSPECT(timepoint);
    OFStandard::deleteFile(path.c_str());
    size_t n = path.find_last_of(".");
    std::string path_raw = path.substr(0,n)+".raw";
    OFStandard::deleteFile(path_raw.c_str());
  }

  // Set a temporary number (higher than the previous)
  std::vector<TimePoint> timepoints;
  tpdb_->LoadVector<TimePoint>(timepoints, odb::query<TimePoint>::patient_id == patient_.id);
  int max = 0;
  for(auto i=timepoints.begin(); i<timepoints.end(); i++) if (i->number > max) max = i->number;
  timepoint.number = max+1;

  // Update the time
  if (patient_.injection_date == "") {
    LOG(FATAL) << "Injection date for the patient " << patient_.name << " is missing.";
  }
  timepoint.time_from_injection_in_hours = syd::DateDifferenceInHours(serie.acquisition_date, patient_.injection_date);

  // Update field
  tpdb_->Update(timepoint);

  // Retrieve corresponding CT
  std::vector<Serie> series;
  db_->LoadVector<Serie>(series,
                         odb::query<Serie>::dicom_frame_of_reference_uid ==
                         serie.dicom_frame_of_reference_uid &&
                         odb::query<Serie>::modality == "CT");
  DD(series.size());
  DD(series[0]);

  // Convert the dicom to mhd
  std::string dicom_filename = db_->GetFullPath(serie);
  std::string mhd_filename   = tpdb_->GetFullPathSPECT(timepoint);
  syd::ConvertDicomToImage(dicom_filename, mhd_filename);

  // Find time order according to existing timepoint
  tpdb_->UpdateAllTimePointNumbers(patient_.id);
}
// --------------------------------------------------------------------
