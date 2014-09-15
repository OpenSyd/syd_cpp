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
std::string syd::ClinicalTrialDatabase::GetFullPath(const Patient & patient)
{
  return get_folder()+patient.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ClinicalTrialDatabase::GetFullPath(const Serie & serie)
{
  std::string p = GetFullPath(GetById<Patient>(serie.patient_id));
  return p+serie.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicalTrialDatabase::CheckPatient(const Patient & patient)
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
void syd::ClinicalTrialDatabase::CheckSerie(const Serie & serie)
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
void syd::ClinicalTrialDatabase::UpdateSerie(Serie & serie)
{
  // Create the path for the acquisition day
  std::string day = serie.acquisition_date.substr(0,10);
  std::string hour = serie.acquisition_date.substr(11,15);
  std::string p = GetFullPath(GetById<Patient>(serie.patient_id))+PATH_SEPARATOR+day+PATH_SEPARATOR;
  if (OFStandard::dirExists(p.c_str())) {
    VLOG(1) << "Folder day date already exist " << p;
  }
  else {
    syd::CreateDirectory(p);
    VLOG(0) << "Create day path " << p;
  }

  // Create the filename (or folder)
  if (serie.modality == "CT") {
    serie.path = day+PATH_SEPARATOR+hour+"_"+serie.modality+"_"+serie.dicom_description;
    std::string path = GetFullPath(serie);
    if (OFStandard::dirExists(path.c_str())) {
      VLOG(0) << "Path already exist " << path;
    }
    else {
      syd::CreateDirectory(path);
      VLOG(0) << "Create path " << path;
    }
  }
  else {
    serie.path = day+PATH_SEPARATOR+hour+"_"+serie.modality+"_"+serie.dicom_description+".dcm";
  }

  // Update the db
  Update(serie);
}
// --------------------------------------------------------------------
