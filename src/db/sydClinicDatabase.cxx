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
syd::ClinicDatabase::ClinicDatabase(std::string name, std::string param)
  :Database(name)
{
   // List all params
  std::istringstream f(param);
  SetFileAndFolder(f);
  OpenSqliteDatabase(filename_, folder_);
  set_check_file_content_level(1);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicDatabase::GetAssociatedCTSerie(IdType serie_id,
                                               std::vector<std::string> & patterns,
                                               Serie & serie)
{
  Serie spect_serie = GetById<Serie>(serie_id);
  Patient patient = GetById<Patient>(spect_serie.patient_id);

  std::vector<Serie> ct_series;
  LoadVector<Serie>(ct_series,
                    odb::query<Serie>::dicom_frame_of_reference_uid ==
                    spect_serie.dicom_frame_of_reference_uid &&
                    odb::query<Serie>::modality == "CT");

  // Check how many ct_series we found
  if (ct_series.size() == 0) {
    LOG(FATAL) << "Error could not find corresponding ct serie with dicom_frame_of_reference_uid = "
               << spect_serie.dicom_frame_of_reference_uid;
  }

  // If we found several ct_series with the dicom_frame_of_reference_uid
  if (ct_series.size() > 1) {
    if (patterns.size() == 0) {
      LOG(FATAL) << "Error we found " << ct_series.size()
                 << " ct_series with the correct dicom_frame_of_reference_uid."
                 << std::endl
                 << "Please use the ct_pattern option to select the one you want.";
    }
    VLOG(3) << "We found " << ct_series.size()
            << " serie(s). Selection with the ct_pattern option...";
    typedef odb::query<Serie> QueryType;
    QueryType q = GetSeriesQueryFromPatterns(patterns);
    q = (QueryType::dicom_frame_of_reference_uid == spect_serie.dicom_frame_of_reference_uid &&
         QueryType::modality == "CT" &&
         QueryType::patient_id == patient.id) && q;
    ct_series.clear();
    LoadVector<Serie>(ct_series, q);
  }

  if (ct_series.size() != 1) {
    LOG(FATAL) << "Error we found " << ct_series.size()
               << " serie(s) with the dicom_frame_of_reference_uid and ct_pattern for this spect.";
  }

  serie = ct_series[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ClinicDatabase::GetSeriePath(IdType id)
{
  Serie serie = GetById<Serie>(id);
  return GetPath(serie);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ClinicDatabase::GetPatientPath(IdType id)
{
  Patient patient = GetById<Patient>(id);
  return GetPath(patient);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ClinicDatabase::GetPath(const Patient & patient)
{
  return get_folder()+patient.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ClinicDatabase::GetPath(const Serie & serie)
{
  std::string p = GetPath(GetById<Patient>(serie.patient_id));
  return p+serie.path;
}
// --------------------------------------------------------------------




// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------
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
      if (GetPath(patient) == GetPath(*i)) {
        LOG(FATAL) << "Error in the DB ! Two patients (ids = " << patient.id  << " and "
                   << i->id << " have the same path '" << i->path;
      }
    }
  }

  // Check the path exist
  std::string path = GetPath(patient);
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
        VLOG(0) << "CheckIntegrity : Error in the DB ! Two series (ids = " << serie.id  << " and "
                   << i->id << " have the same dicom_uid '" << i->dicom_uid;
      }
      if (GetPath(serie) == GetPath(*i)) {
        VLOG(0) << "CheckIntegrity : Error in the DB ! Two series (ids = " << serie.id  << " and "
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
  std::string path = GetPath(serie);
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
    VLOG(0) << "CheckIntegrity : Error serie " << serie.id << " the number of files is supposed to be " << serie.number_of_files
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
      VLOG(0) << "CheckIntegrity : Error serie " << serie.id << " modality is supposed to be " << serie.modality
                 << " but I read " << modality << " in the file " << path << " " << *i;
    }

    // Check uid
    std::string uid = GetTagValueString(dset, "SeriesInstanceUID");
    if (serie.dicom_uid != uid) {
      VLOG(0) << "CheckIntegrity : Error serie " << serie.id << " uid is supposed to be " << serie.dicom_uid
                 << " but I read " << uid << " in the file " << path << " " << *i;
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicDatabase::CheckSerie_NM(const Serie & serie)
{
  // Check if the file exist
  std::string path = GetPath(serie);
  if (!OFStandard::fileExists(path.c_str())) {
    VLOG(0) << "CheckIntegrity : Error serie " << serie.id << " : the file " << path << " does not exist.";
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
    VLOG(0) << "CheckIntegrity : Error serie " << serie.id << " modality is supposed to be " << serie.modality
               << " but I read " << modality << " in the file " << path;
  }

  // Check uid
  std::string uid = GetTagValueString(dset, "SOPInstanceUID");
  if (serie.dicom_uid != uid) {
    VLOG(0) << "CheckIntegrity : Error serie " << serie.id << " uid is supposed to be " << serie.dicom_uid
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
  std::string p = GetPath(GetById<Patient>(serie.patient_id))+PATH_SEPARATOR+day+PATH_SEPARATOR;
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
    std::string path = GetPath(serie);
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
void syd::ClinicDatabase::Dump(std::ostream & os, std::vector<std::string> & args)
{
  // Get the command
  std::string cmd = args[0];
  std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

  // Get the patients
  std::string patient_name = args[1];
  std::vector<Patient> patients;
  GetPatientsByName(patient_name, patients);

  // Get the patterns
  std::vector<std::string> patterns;
  for(auto i=2; i<args.size(); i++) patterns.push_back(args[i]);

  // Warning if cmd is 'patient' because patterns is not used
  if (cmd.find("patient") != std::string::npos) {
    if (patterns.size() != 0) {
      std::string p;
      for(auto i=patterns.begin(); i != patterns.end(); i++) p = p+*i+" ";
      LOG(WARNING) << "The string patterns you provide ('" << p << "') are ignored.";
    }
  }

  // Check cmd
  if ((cmd.find("serie") == std::string::npos) &&
      (cmd.find("patient") == std::string::npos)) {
    LOG(FATAL) << "Error Dump for ClinicDatabase type must be 'serie' or 'patient'";
  }

  // cmd = patient : loop over patient to display
  if (cmd.find("patient")!= std::string::npos)
    for(auto i:patients) os << Print(i,2) << std::endl;

  // cmd = serie : loop over all series for all patients
  if (cmd.find("serie")!= std::string::npos) {
    for(auto patient:patients) {
      typedef odb::query<Serie> QueryType;
      QueryType q = GetSeriesQueryFromPatterns(patterns);
      q = (QueryType::patient_id == patient.id) && q;
      std::vector<Serie> series;
      LoadVector<Serie>(series, q);

      // Sort by acquisition_date
      std::sort(series.begin(), series.end(),
                [&](Serie a, Serie b) { return syd::IsBefore(a.acquisition_date, b.acquisition_date); }  );

      // Print all series
      for(auto i:series) os << Print(i) << std::endl;

      // Print serie ids
      os << patient.name << " " << patient.synfrizz_id << " (total " << series.size() << ") ";
      for(auto i: series) std::cout << i.id << " ";
      os << std::endl;
    }
  }
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


// --------------------------------------------------------------------
void syd::ClinicDatabase::CheckIntegrity(std::vector<std::string> & args)
{
  // Check args, get the patients
  std::string patient_name;
  if (args.size() == 0) patient_name = "all";
  else patient_name = args[0];
  std::vector<Patient> patients;
  GetPatientsByName(patient_name, patients);

  // Loop over the patients
  for(auto i:patients) CheckIntegrity(i);
}

// --------------------------------------------------------------------
void syd::ClinicDatabase::CheckIntegrity(const Patient & patient)
{
  // Part 1
  VLOG(0) << "Part 1: from DB to files";
  std::vector<Serie> series;
  LoadVector<Serie>(series, odb::query<Serie>::patient_id == patient.id);
  VLOG(1) << "Found " << series.size() << " series. Checking ...";
  for(auto i=series.begin(); i<series.end(); i++) {
    VLOG(2) << "Checking serie " << i->id << " " << i->path;
    CheckSerie(*i);
  }

  // Part 2
  VLOG(0) << "Part 2 : from files to DB (could be long)";
  // Search for all folders in patient folder
  std::string folder = GetPath(patient);

  // For all folders, find dicom
  if (!OFStandard::dirExists(folder.c_str())) {
    LOG(FATAL) << "The folder " << folder << " does not exist.";
  }
  OFList<OFString> inputFiles;
  inputFiles.clear();
  OFString scanPattern = "*dcm";
  OFString dirPrefix = "";
  OFBool recurse = OFTrue;
  size_t found =
    OFStandard::searchDirectoryRecursively(folder.c_str(), inputFiles, scanPattern, dirPrefix, recurse);
  VLOG(1) << "Found " << inputFiles.size() << " files, checking...";

  // for all dicom check in db
  int n=0;
  int m = inputFiles.size()/10;
  for(auto i=inputFiles.begin(); i != inputFiles.end(); i++) {
    VLOG_EVERY_N(m, 1) << ++n << "/10";
    CheckFile(*i);
  }
  VLOG(0) << "Done, everything seems correct.";

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClinicDatabase::CheckFile(OFString filename)
{
  DcmFileFormat dfile;
  bool b = syd::OpenDicomFile(filename.c_str(), true, dfile);
  DcmObject *dset = dfile.getDataset();
  if (!b) {
    LOG(FATAL) << "Could not open the file " << filename;
  }
  std::string k;
  std::string seriesUID = GetTagValueString(dset, "SeriesInstanceUID");
  std::string SOP_UID = GetTagValueString(dset, "SOPInstanceUID");
  std::string modality = GetTagValueString(dset, "Modality");
  if (modality == "CT") k = seriesUID;
  else k = SOP_UID;

  // Find the serie
  Serie serie;
  b = GetIfExist<Serie>(odb::query<Serie>::dicom_uid == k, serie);
  if (!b) {
    VLOG(0) << "CheckIntegrity : Error the file " << filename << " contains a dicom which is not in the db.";
  }
}
// --------------------------------------------------------------------
