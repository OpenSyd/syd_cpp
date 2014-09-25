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
#include "sydClinicDatabase.h"

// --------------------------------------------------------------------
syd::ClinicDatabase::ClinicDatabase(std::string name):
  Database("ClinicDatabase", name)
{
  set_check_file_content_level(1);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ClinicDatabase::GetFullPath(const Patient & patient)
{
  return get_folder()+patient.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ClinicDatabase::GetFullPath(const Serie & serie)
{
  std::string p = GetFullPath(GetById<Patient>(serie.patient_id));
  return p+serie.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicDatabase::GetPatientsByName(std::string patient_name,
                                            std::vector<Patient> & patients)
{
  if (patient_name == "all" or patient_name == "") {
    LoadVector<Patient>(patients);
    // Sort by acquisition_date
    std::sort(patients.begin(), patients.end(),
              [&](Patient a, Patient b) { return syd::IsBefore(a.injection_date, b.injection_date); }  );
  }
  else { // Get only one patient
    Patient patient;
    if (!GetIfExist<Patient>(odb::query<Patient>::name == patient_name, patient)) {
      LOG(FATAL) << "Error, the patient " << patient_name << " does not exist";
    }
    patients.push_back(patient);
  }

  // Error if not patient found
  if (patients.size() == 0) {
    LOG(FATAL) << "Error not patient found with '" << patient_name << "'.";
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicDatabase::CheckPatient(const Patient & patient)
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
void syd::ClinicDatabase::CheckSerie(const Serie & serie)
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
                   << i->id << " have the same path '" << i->path << "." << std::endl
                   << i->dicom_uid;
      }
    }
  }

  // Different check according to modality
  if (serie.modality == "CT") CheckSerie_CT(serie);
  else CheckSerie_NM(serie);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicDatabase::CheckSerie_CT(const Serie & serie)
{
  // Check the path exist
  std::string path = GetFullPath(serie);
  if (!OFStandard::dirExists(path.c_str())) {
    LOG(FATAL) << "Error serie " << serie.id << " : the folder " << path << " does not exist.";
  }

  // Check file content (slow)
  if (check_file_content_level_ == 0) return;

  // Look in the folder
  OFString scanPattern = "*dcm";
  OFString dirPrefix = "";
  OFBool recurse = OFFalse;
  OFList<OFString> inputFiles;
  size_t found =
    OFStandard::searchDirectoryRecursively(path.c_str(), inputFiles, scanPattern, dirPrefix, recurse);

  // Check the number of files
  int n = inputFiles.size();
  if (serie.number_of_files != n) {
    LOG(FATAL) << "Error serie " << serie.id << " the number of files is supposed to be " << serie.number_of_files
               << " but I found " << n << " files int the folder " << path;
  }
  if (check_file_content_level_ == 1) return;

  for(auto i=inputFiles.begin(); i!=inputFiles.end(); i++) {
    // Open the files
    DcmFileFormat dfile;
    syd::OpenDicomFile(i->c_str(), false, dfile);
    DcmObject *dset = dfile.getDataset();

    // Check modality
    std::string modality = GetTagValueString(dset, "Modality");
    if (serie.modality != modality) {
      LOG(FATAL) << "Error serie " << serie.id << " modality is supposed to be " << serie.modality
                 << " but I read " << modality << " in the file " << path << " " << *i;
    }

    // Check uid
    std::string uid = GetTagValueString(dset, "SeriesInstanceUID");
    if (serie.dicom_uid != uid) {
      LOG(FATAL) << "Error serie " << serie.id << " uid is supposed to be " << serie.dicom_uid
                 << " but I read " << uid << " in the file " << path << " " << *i;
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicDatabase::CheckSerie_NM(const Serie & serie)
{
  // Check if the file exist
  std::string path = GetFullPath(serie);
  if (!OFStandard::fileExists(path.c_str())) {
    LOG(FATAL) << "Error serie " << serie.id << " : the file " << path << " does not exist.";
  }

  // Check file content (slow)
  if (check_file_content_level_ == 0) return;
  DcmFileFormat dfile;
  syd::OpenDicomFile(path, false, dfile);
  DcmObject *dset = dfile.getDataset();

  // Check modality
  std::string modality = GetTagValueString(dset, "Modality");
  if (modality != "CT") modality = "NM";
  if (serie.modality != modality) {
    LOG(FATAL) << "Error serie " << serie.id << " modality is supposed to be " << serie.modality
               << " but I read " << modality << " in the file " << path;
  }

  // Check uid
  std::string uid = GetTagValueString(dset, "SOPInstanceUID");
  if (serie.dicom_uid != uid) {
    LOG(FATAL) << "Error serie " << serie.id << " uid is supposed to be " << serie.dicom_uid
               << " but I read " << uid << " in the file " << path;
  }

}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::ClinicDatabase::UpdateSerie(Serie & serie)
{

  // Create or get the path for the acquisition day
  std::string day = serie.acquisition_date.substr(0,10);
  std::string hour = serie.acquisition_date.substr(11,15);
  std::string p = GetFullPath(GetById<Patient>(serie.patient_id))+PATH_SEPARATOR+day+PATH_SEPARATOR;
  if (OFStandard::dirExists(p.c_str())) {
    VLOG(2) << "Folder day date already exist " << p;
  }
  else {
    syd::CreateDirectory(p);
    VLOG(1) << "Create day path " << p;
  }

  // Create the filename (for NM) or folder (for CT)
  if (serie.modality == "CT") {
    serie.path = day+PATH_SEPARATOR+hour+"_"+serie.modality+"_"+serie.dicom_series_desc;
    std::string path = GetFullPath(serie);
    if (OFStandard::dirExists(path.c_str())) {
      VLOG(2) << "Path already exist " << path;
    }
    else {
      syd::CreateDirectory(path);
      VLOG(1) << "Create path " << path;
    }
  }
  else {
    serie.path = day+PATH_SEPARATOR+hour+"_"+serie.dicom_uid+".dcm";
  }

  // Update the db
  Update(serie);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
odb::query<Serie> syd::ClinicDatabase::GetSeriesQueryFromPatterns(std::vector<std::string> patterns)
{
  typedef odb::query<Serie> QueryType;
  QueryType q = (QueryType::id != 0); // required initialization.
  for(auto i=patterns.begin(); i<patterns.end(); i++)
    AndSeriesQueryFromPattern(q, *i);
  return q;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicDatabase::AndSeriesQueryFromPattern(odb::query<Serie> & q, std::string pattern)
{
  typedef odb::query<Serie> QueryType;
  pattern = "%"+pattern+"%";
  q = q &&
    (QueryType::dicom_dataset_name.like(pattern) ||
     QueryType::dicom_image_id.like(pattern) ||
     QueryType::modality.like(pattern) ||
     QueryType::dicom_series_desc.like(pattern) ||
     QueryType::reconstruction_date.like(pattern) ||
     QueryType::dicom_study_desc.like(pattern));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ClinicDatabase::Print(Patient patient, int level)
{
  std::stringstream ss;
  ss << patient.name << " "
     << patient.synfrizz_id << "\t"
     << patient.injection_date;
  if (level > 0) {
    ss << "\t"  << (patient.was_treated ? "90Y":"no ") << "\t"
       << patient.injected_quantity_in_MBq << " MBq";
  }
  if (level > 1) {
    // Get the number of series for this patient
    std::vector<Serie> series;
    LoadVector<Serie>(series, odb::query<Serie>::patient_id == patient.id);
    int n = series.size();
    int nb_ct = 0;
    int nb_nm = 0;
    ss << "\t";
    for(auto i=series.begin(); i != series.end(); i++) {
      if (i->modality == "CT") nb_ct++;
      else nb_nm++;
    }
    ss << nb_ct << " CTs \t" << nb_nm << " NMs";
  }
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ClinicDatabase::Print(Serie serie)
{
  Patient patient = GetById<Patient>(serie.patient_id);
  std::stringstream ss;
  std::cout << patient.name << " "
            << patient.synfrizz_id << " "
            << serie.id << " "
            << serie.acquisition_date << " "
            << serie.reconstruction_date << " "
            << serie.dicom_study_desc << "\t"
            << serie.dicom_series_desc << "\t"
            << serie.dicom_dataset_name << "\t"
            << serie.dicom_image_id << "\t"
            << serie.dicom_instance_number;
    return ss.str();
}
// --------------------------------------------------------------------
