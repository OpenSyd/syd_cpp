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
#include "sydClinicalTrialDatabase.h"

// --------------------------------------------------------------------
void syd::ClinicalTrialDatabase::OpenDatabase()
{
  // Get DB filename
  char * bdb = getenv ("SYD_SYNFRIZZ_DB");
  if (bdb == NULL) LOG(FATAL) << " please set SYD_SYNFRIZZ_DB environment variable.";
  std::string filename = std::string(bdb);

  // Get Database folder
  char * b =getenv ("SYD_SYNFRIZZ_IMAGE_FOLDER");
  if (b == NULL) LOG(FATAL) << " please set SYD_SYNFRIZZ_IMAGE_FOLDER environment variable.";
  std::string folder = std::string(b)+"/";

  // Open
  Database::OpenDatabase(filename, folder);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ClinicalTrialDatabase::GetFullPath(Patient & patient)
{
  return get_folder()+patient.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ClinicalTrialDatabase::GetFullPath(Study & study)
{
  std::string p = GetFullPath(GetById<Patient>(study.patient_id));
  return p+study.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ClinicalTrialDatabase::GetFullPath(Serie & serie)
{
  std::string p = GetFullPath(GetById<Study>(serie.study_id));
  return p+serie.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicalTrialDatabase::AddPatient(std::string name, Patient & patient)
{
  // Get the patient if already exist
  if (GetIfExist<Patient>(odb::query<Patient>::name == name, patient)) return;

  // Create the patient
  patient.name = name;
  patient.weight_in_kg = 0.0;
  patient.synfrizz_id = 0;
  patient.was_treated = false;
  patient.injection_date = "";
  patient.injected_quantity_in_MBq = 0.0;

  // Create the path
  patient.path = name+PATH_SEPARATOR;
  std::string path = GetFullPath(patient);

  // Check the folder
  if (OFStandard::dirExists(path.c_str())) {
    LOG(FATAL) << "Patient " << name
               << " does not exist, but the folder "
               << path << " already exist. Abort.";
  }

  syd::CreateDirectory(path);

  // Update the db
  Insert(patient);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicalTrialDatabase::AddStudy(const Patient & patient, std::string uid, std::string date, Study & study)
{
  // Get the study if already exist
  if (GetIfExist<Study>(odb::query<Study>::dicom_uid == uid, study)) return;

  // Create the study
  study.patient_id = patient.id;
  study.dicom_uid = uid;
  study.date = date;
  study.path = study.date+PATH_SEPARATOR;

  // Create the path
  std::string path = GetFullPath(study);

  // Check the folder
  if (OFStandard::dirExists(path.c_str())) {
    LOG(FATAL) << "Study " << uid
               << " does not exist, but the folder "
               << path << " already exist. Abort.";
  }

  syd::CreateDirectory(path);

  // Update the db
  Insert(study);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicalTrialDatabase::CheckStudy(Study & study)
{
  // Check the DB : single dicom_uid, path
  std::vector<Study> studies;
  LoadVector<Study>(studies);
  for(auto i=studies.begin(); i<studies.end(); i++) {
    if (study.id != i->id) {
      if (study.dicom_uid == i->dicom_uid) {
        LOG(FATAL) << "Error in the DB ! Two studies (ids = " << study.id  << " and "
                   << i->id << " have the same dicom_uid '" << i->dicom_uid;
      }
      if (GetFullPath(study) == GetFullPath(*i)) {
        LOG(FATAL) << "Error in the DB ! Two studies (ids = " << study.id  << " and "
                   << i->id << " have the same path '" << i->path;
      }
    }
  }

  // Check the path exist
  std::string path = GetFullPath(study);
  if (!OFStandard::dirExists(path.c_str())) {
    LOG(FATAL) << "Error for study id " << study.id << " the folder " << path
               << " does not exist.";
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicalTrialDatabase::CheckPatient(Patient & patient)
{
  // Check the DB : single name, single path, single synfrizz_id
  std::vector<Patient> patients;
  LoadVector<Patient>(patients);
  for(auto i=patients.begin(); i<patients.end(); i++) {
    if (patient.id != i->id) {
      if (patient.name == i->name) {
        LOG(FATAL) << "Error in the DB ! Two patients (ids = " << patient.id  << " and "
                   << i->id << " have the same name '" << i->name;
      }
      if ((patient.synfrizz_id == i->synfrizz_id) && (i->synfrizz_id != 0)) {
        LOG(FATAL) << "Error in the DB ! Two patients (ids = " << patient.id  << " and "
                   << i->id << " have the same synfrizz id '" << i->synfrizz_id;
      }
      if (GetFullPath(patient) == GetFullPath(*i)) {
        LOG(FATAL) << "Error in the DB ! Two patients (ids = " << patient.id  << " and "
                   << i->id << " have the same path '" << i->path;
      }
    }
  }

  // Check the path exist
  std::string path = GetFullPath(patient);
  if (!OFStandard::dirExists(path.c_str())) {
    LOG(FATAL) << "Error for patient id " << patient.id << " the folder " << path
               << " does not exist.";
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicalTrialDatabase::CheckSerie(Serie & serie)
{
  // Check the DB : single dicom_uid, path
  std::vector<Serie> series;
  LoadVector<Serie>(series);
  for(auto i=series.begin(); i<series.end(); i++) {
    if (serie.id != i->id) {
      if (serie.dicom_uid == i->dicom_uid) {
        LOG(FATAL) << "Error in the DB ! Two series (ids = " << serie.id  << " and "
                   << i->id << " have the same dicom_uid '" << i->dicom_uid;
      }
      if (GetFullPath(serie) == GetFullPath(*i)) {
        LOG(FATAL) << "Error in the DB ! Two series (ids = " << serie.id  << " and "
                   << i->id << " have the same path '" << i->path;
      }
    }
  }

  // Check the path exist
  std::string path = GetFullPath(serie);
  if (!OFStandard::dirExists(path.c_str())) {
    LOG(FATAL) << "Error for serie id " << serie.id << " the folder " << path
               << " does not exist.";
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicalTrialDatabase::AddSerie(const Study & study,
                                          std::string description,
                                          std::string uid,
                                          Serie & serie)
{
  // Get the serie if already exist
  if (GetIfExist<Serie>(odb::query<Serie>::dicom_uid == uid, serie)) return;

  // Update the serie
  serie.study_id = study.id;
  serie.dicom_uid = uid;
  serie.dicom_description = description;
  serie.acquisition_date = "";

  // Create the path
  serie.path = description+"__"+uid+PATH_SEPARATOR;
  std::string path = GetFullPath(serie);

  // Check the folder
  if (OFStandard::dirExists(path.c_str())) {
    LOG(FATAL) << "Serie " << uid << " does not exist, but the folder "
               << path << "exist. Abort.";
  }
  syd::CreateDirectory(path);

  // Update the db
  Insert(serie);
}
// --------------------------------------------------------------------
