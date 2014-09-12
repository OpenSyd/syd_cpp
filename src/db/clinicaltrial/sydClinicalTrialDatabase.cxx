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
void syd::ClinicalTrialDatabase::AddPatient(std::string name, Patient & patient)
{
  // Get the patient if already exist
  if (GetIfExist<Patient>(odb::query<Patient>::name == name, patient)) return;

  // Update the patient
  patient.name = name;

  // Create the path
  patient.path = name+PATH_SEPARATOR;
  std::string path = get_folder()+patient.path;
  DD(path);

  // Check the folder
  if (OFStandard::dirExists(path.c_str())) {
    LOG(FATAL) << "Patient " << name
               << " does not exist, but the folder "
               << path << " already exist. Abort.";
  }

  syd::CreateDirectory(patient.path);

  // Update the db
  Insert(patient);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicalTrialDatabase::CheckPatient(Patient & patient)
{
  DD(" TODO CheckPatients");
  // db : single name, single initials

  // folders : single path exist

  // unique synf id ?

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicalTrialDatabase::CheckSerie(Serie & serie)
{
  DD("TODO CheckSerie");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicalTrialDatabase::AddSerie(Patient p,
                                          std::string description,
                                          std::string uid,
                                          Serie & s)
{
  // Get the serie if already exist
  if (GetIfExist<Serie>(odb::query<Serie>::dicom_uid == uid, s)) return;

  // Update the serie
  s.patient_id = p.id;
  s.dicom_uid = uid;
  s.dicom_description = description;

  // Create the path
  s.path = description+"__"+uid+PATH_SEPARATOR;
  std::string path = get_folder()+p.path+s.path;
  DD(path);

  // Check the folder
  if (OFStandard::dirExists(path.c_str())) {
    LOG(FATAL) << "Serie " << uid << " does not exist, but the folder "
               << path << "exist. Abort.";
  }
  syd::CreateDirectory(path);

  // Update the db
  Insert(s);
}
// --------------------------------------------------------------------
