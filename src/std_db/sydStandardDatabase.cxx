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
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
void syd::StandardDatabase::CreateTables()
{
  //  syd::Database::CreateTables();
  AddTable<syd::Patient>();
  AddTable<syd::Radionuclide>();
  AddTable<syd::File>();
  AddTable<syd::Tag>();

  AddTable<syd::Injection>();

  AddTable<syd::DicomSerie>();
  AddTable<syd::DicomFile>();

  AddTable<syd::Timepoint>();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::StandardDatabase::QueryPatientByNameOrStudyId(const std::string & arg,
                                                      Patient & p, bool fatalIfNotFound)
{
  // Check if name
  bool found = false;
  odb::query<Patient> q = odb::query<Patient>::name == arg;
  if (Count<Patient>(q) == 1) p = QueryOne(q);
  else {
    q = odb::query<Patient>::study_id == atoi(arg.c_str());
    if (Count<Patient>(q) == 1) {
      p = QueryOne(q);
      found = true;
    }
    else {
      if (fatalIfNotFound) {
        LOG(FATAL) << "Could not find patient with name or study_id equal to '"
                   << arg << "'.";
      }
    }
  }
  return found;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::QueryPatientsByNameOrStudyId(const std::string & arg,
                                                       std::vector<syd::Patient> & patients)
{
  if (arg == "all") Query<Patient>(patients);
  else {
    Patient p;
    bool b = QueryPatientByNameOrStudyId(arg, p, false);
    if (b) patients.push_back(p);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::StandardDatabase::QueryInjectionByNameOrId(const Patient & patient,
                                                     const std::string & arg,
                                                     Injection & p, bool fatalIfNotFound)
{
  bool found = false;
  std::string s = "%"+arg+"%";
  odb::query<Injection> q =
    odb::query<Injection>::radionuclide->name.like(s) and
    odb::query<Injection>::patient->id == patient.id;
  std::vector<syd::Injection> injections;
  Query(q, injections);

  auto n = injections.size();

  if (n == 1) {
    p = injections[0];
    return true;
  }

  if (fatalIfNotFound and n > 1) {
    LOG(FATAL) << "Several injections match '" << arg
               << "' for the patient '" << patient.name << "', abort.";
  }
  if (fatalIfNotFound and n==0) {
    LOG(FATAL) << "Could not find injection with name or id equal to '"
               << arg << "' for the patient " << patient.name << ".";
  }

  return false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TableElement * syd::StandardDatabase::InsertFromArg(const std::string & table_name,
                                                  std::vector<std::string> & arg)
{
  if (table_name == syd::Injection::GetTableName()) return InsertInjection(arg);
  //  if (table_name == syd::DicomSerie::GetTableName()) return InsertDicomSerie(arg);
  return syd::Database::InsertFromArg(table_name, arg);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Injection * syd::StandardDatabase::InsertInjection(std::vector<std::string> & arg)
{
  if (arg.size() < 4) {
    LOG(FATAL) << "Provide <patient> <radionuclide> <date> <activity_in_MBq>. "
               << std::endl
               << " <patient> can be the study_id or the name";
  }

  // Check date
  std::string date = arg[2];
  if (!syd::IsDateValid(date)) {
    LOG(FATAL) << "The date '" << date
               << "' is not valid. It must be something like '2015-04-01 10:00'";
  }

  // Get the patient and create the Injection
  Patient p;
  QueryPatientByNameOrStudyId(arg[0], p);

  // Check other injections for this patient
  std::vector<Injection> injections;
  Query<Injection>(odb::query<Injection>::patient->id == p.id, injections);
  for(auto i:injections) {
    double d = fabs(DateDifferenceInHours(date, i.date));
    if (d<3) {
      LOG(FATAL) << "Error: another injection already exist for this patient, with a close date (lower than 3 hours)."
                 << std::endl << "Injection: " << i;
    }
  }

  // Get Radionuclide
  syd::Radionuclide radionuclide;
  radionuclide = QueryOne<syd::Radionuclide>(odb::query<Radionuclide>::name.like(arg[1]));

  // Create the new injection
  Injection * injection = new Injection;
  injection->radionuclide = std::make_shared<Radionuclide>(radionuclide);
  injection->date = date;
  injection->activity_in_MBq = atof(arg[3].c_str());
  // This is how to insert the link to the patient object
  injection->patient = std::make_shared<Patient>(p);
  syd::Database::Insert<syd::Injection>(*injection);
  return injection;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::DumpDicom(std::ostream & os,
                                    syd::Patient & patient,
                                    const std::vector<std::string> & patterns,
                                    double max_time_diff)
{
  // init the table to output results
  syd::PrintTable table;
  table.AddColumn("#id", 4);
  table.AddColumn("date", 18);
  table.AddColumn("mod", 4);
  table.AddColumn("recon", 18);
  table.AddColumn("inj",12);
  table.AddColumn("desc",110);
  table.Init();

  // Build the query
  odb::query<DicomSerie> q = odb::query<DicomSerie>::patient->id == patient.id;
  for(auto p:patterns) {
    std::string s = "%"+p+"%";
    q = q and (odb::query<DicomSerie>::dicom_description.like(s)
               or odb::query<DicomSerie>::dicom_modality.like(s)
               or odb::query<DicomSerie>::acquisition_date.like(s)
               or odb::query<DicomSerie>::reconstruction_date.like(s)
               //  or odb::query<DicomSerie>::injection.radionuclide.name.like(s)
               );
  }

  // Sort by acquisition_date then reconstruction_date
  q = q + "ORDER BY" + odb::query<DicomSerie>::acquisition_date +
    "," + odb::query<DicomSerie>::reconstruction_date;

  // Perform the query
  std::vector<syd::DicomSerie> series;
  Query<DicomSerie>(q, series);
  if (series.size() == 0) return;

  // Dump all information
  std::string previous = series[0].acquisition_date;
  for(auto s:series) {
    double diff = syd::DateDifferenceInHours(s.acquisition_date, previous);
    if (diff > max_time_diff) table.SkipLine();
    table << s.id
          << s.acquisition_date
          << s.dicom_modality
          << s.reconstruction_date
          << s.injection->radionuclide->name
          << s.dicom_description;
    previous = s.acquisition_date;
  }
  table.Print(os);

  // List of ids
  for(auto s:series) os << s.id << " ";
  os << std::endl;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetAbsoluteFolder(const DicomSerie & serie)
{
  Patient patient = QueryOne<Patient>(serie.patient->id);
  std::string f = GetAbsoluteFolder(patient);
  if (!syd::DirExists(f)) syd::CreateDirectory(f); // create patient dir
  std::string d = serie.acquisition_date;
  //  syd::Replace(d, " ", "_");
  // remove the hour and keep y m d
  d = d.substr(0, 10);
  f = f+PATH_SEPARATOR+d;
  if (!syd::DirExists(f)) syd::CreateDirectory(f); // create date dir
  f = f+PATH_SEPARATOR+serie.dicom_modality;
  if (!syd::DirExists(f)) syd::CreateDirectory(f); // create modality dir
  return f;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetAbsoluteFolder(const Patient & patient)
{
  std::string f = GetAbsoluteDBFolder();
  f = f+PATH_SEPARATOR+patient.name;
  return f;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::DumpTimepoint(const std::vector<std::string> & args, std::ostream & os, bool verboseFlag)
{

  std::vector<syd::Timepoint> timepoints;
  Query(timepoints);

  for(auto t:timepoints) {
    DD(t);
    for(auto d:t.dicoms) {
      std::cout << "\t " << *d << std::endl;
    }
  }

}
// --------------------------------------------------------------------


/*
// --------------------------------------------------------------------
void syd::StandardDatabase::GetAssociatedCTSerie(IdType serie_id,
                                               std::vector<std::string> & patterns,
                                               Serie & serie)
{
  Serie spect_serie = GetById<Serie>(serie_id);
  Patient patient = GetById<Patient>(spect_serie.patient_id_fk);

  std::vector<Serie> ct_series;
  LoadVector<Serie>(odb::query<Serie>::dicom_frame_of_reference_uid ==
                    spect_serie.dicom_frame_of_reference_uid &&
                    odb::query<Serie>::modality == "CT", ct_series);

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
    ELOG(3) << "We found " << ct_series.size()
            << " ct series, so we try to select one with the ct_pattern option...";
    typedef odb::query<Serie> QueryType;
    QueryType q = GetSeriesQueryFromPatterns(patterns);
    q = (QueryType::dicom_frame_of_reference_uid == spect_serie.dicom_frame_of_reference_uid &&
         QueryType::modality == "CT" &&
         QueryType::patient_id_fk == patient.id) && q;
    ct_series.clear();
    LoadVector<Serie>(q, ct_series);
  }

  if (ct_series.size() != 1) {
    LOG(FATAL) << "Error we found " << ct_series.size()
               << " serie(s) with the dicom_frame_of_reference_uid and ct_pattern for this spect.";
  }

  serie = ct_series[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetSeriePath(IdType serie_id)
{
  Serie serie = GetById<Serie>(serie_id);
  return GetPath(serie);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetPatientPath(IdType patient_id_fk)
{
  Patient patient = GetById<Patient>(patient_id_fk);
  return GetPath(patient);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetPath(const Patient & patient)
{
  return get_folder()+patient.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetPath(const Serie & serie)
{
  std::string p = GetPath(GetById<Patient>(serie.patient_id_fk));
  return p+serie.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
Patient syd::StandardDatabase::GetPatient(const Serie & serie)
{
  return GetById<Patient>(serie.patient_id_fk);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::GetPatientsByName(const std::string & patient_name,
                                            std::vector<Patient> & patients)
{
  if (patient_name == "all" or patient_name == "") {
    LoadVector<Patient>(patients);
  }
  else {
    std::vector<std::string> names;
    syd::GetWords(patient_name, names);
    for(auto s:names) {
      Patient patient;
      if (!GetIfExist<Patient>(odb::query<Patient>::name == s, patient)) {
        LOG(FATAL) << "Error, the patient <" << s << "> does not exist";
      }
      patients.push_back(patient);
    }
  }

  // Sort by study_id
  std::sort(begin(patients), end(patients),
            [this](Patient a, Patient b) {
              return a.study_id < b.study_id; }  );

  // Error if not patient found
  if (patients.size() == 0) {
    LOG(FATAL) << "Error no patient found with '" << patient_name << "'.";
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
Patient syd::StandardDatabase::GetPatientByName(const std::string & patient_name)
{
  if (patient_name == "all" or patient_name == "") {
    LOG(FATAL) << "Error, please provide a patient name not 'all' or empty name.";
  }
  Patient patient;
  if (!GetIfExist<Patient>(odb::query<Patient>::name == patient_name, patient)) {
    LOG(FATAL) << "Error, the patient " << patient_name << " does not exist";
  }
  return patient;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::CheckPatient(const Patient & patient)
{
  // Check the DB : single name, single path, single study_id
  std::vector<Patient> patients;
  LoadVector<Patient>(patients);
  for(auto i=patients.begin(); i<patients.end(); i++) {
    if (patient.id != i->id) {
      if (patient.name == i->name) {
        LOG(FATAL) << "Error in the DB ! Two patients (ids = " << patient.id  << " and "
                   << i->id << " have the same name '" << i->name;
      }
      if ((patient.study_id == i->study_id) && (i->study_id != 0)) {
        LOG(FATAL) << "Error in the DB ! Two patients (ids = " << patient.id  << " and "
                   << i->id << " have the same synfrizz id '" << i->study_id;
      }
      if (GetPath(patient) == GetPath(*i)) {
        LOG(FATAL) << "Error in the DB ! Two patients (ids = " << patient.id  << " and "
                   << i->id << " have the same path '" << i->path;
      }
    }
  }

  // Check the path exist
  std::string path = GetPath(patient);
  if (!syd::DirExists(path)) {
    LOG(FATAL) << "Error for patient id " << patient.id << " the folder " << path
               << " does not exist.";
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::CheckSerie(const Serie & serie)
{
  // Check the DB : single dicom_uid, path
  std::vector<Serie> series;
  LoadVector<Serie>(series);
  for(auto i=series.begin(); i<series.end(); i++) {
    if (serie.id != i->id) {
      if (serie.dicom_uid == i->dicom_uid) {
        ELOG(0) << "CheckIntegrity : Error in the DB ! Two series (ids = " << serie.id  << " and "
                   << i->id << " have the same dicom_uid '" << i->dicom_uid;
      }
      if (GetPath(serie) == GetPath(*i)) {
        ELOG(0) << "CheckIntegrity : Error in the DB ! Two series (ids = " << serie.id  << " and "
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
void syd::StandardDatabase::CheckSerie_CT(const Serie & serie)
{
  // Check the path exist
  std::string path = GetPath(serie);
  if (!syd::DirExists(path)) {
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
    ELOG(0) << "CheckIntegrity : Error serie " << serie.id << " the number of files is supposed to be " << serie.number_of_files
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
      ELOG(0) << "CheckIntegrity : Error serie " << serie.id << " modality is supposed to be " << serie.modality
                 << " but I read " << modality << " in the file " << path << " " << *i;
    }

    // Check uid
    std::string uid = GetTagValueString(dset, "SeriesInstanceUID");
    if (serie.dicom_uid != uid) {
      ELOG(0) << "CheckIntegrity : Error serie " << serie.id << " uid is supposed to be " << serie.dicom_uid
                 << " but I read " << uid << " in the file " << path << " " << *i;
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::CheckSerie_NM(const Serie & serie)
{
  // Check if the file exist
  std::string path = GetPath(serie);
  if (!syd::FileExists(path)) {
    ELOG(0) << "CheckIntegrity : Error serie " << serie.id << " : the file " << path << " does not exist.";
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
    ELOG(0) << "CheckIntegrity : Error serie " << serie.id << " modality is supposed to be " << serie.modality
               << " but I read " << modality << " in the file " << path;
  }

  // Check uid
  std::string uid = GetTagValueString(dset, "SOPInstanceUID");
  if (serie.dicom_uid != uid) {
    ELOG(0) << "CheckIntegrity : Error serie " << serie.id << " uid is supposed to be " << serie.dicom_uid
               << " but I read " << uid << " in the file " << path;
  }

}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::StandardDatabase::UpdateSerie(Serie & serie)
{
  // Create or get the path for the acquisition day
  std::string day = serie.acquisition_date.substr(0,10);
  std::string hour = serie.acquisition_date.substr(11,15);
  std::string p = GetPath(GetById<Patient>(serie.patient_id_fk))+PATH_SEPARATOR+day+PATH_SEPARATOR;
  if (syd::DirExists(p)) {
    ELOG(2) << "Folder day date already exist " << p;
  }
  else {
    syd::CreateDirectory(p);
    ELOG(1) << "Create day path " << p;
  }

  // Create the filename (for NM) or folder (for CT)
  if (serie.modality == "CT") {
    serie.path = day+PATH_SEPARATOR+hour+"_"+serie.modality+"_"+serie.dicom_series_desc;
    std::string path = GetPath(serie);
    if (syd::DirExists(path)) {
      ELOG(2) << "Path already exist " << path;
    }
    else {
      syd::CreateDirectory(path);
      ELOG(1) << "Create path " << path;
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
odb::query<Serie> syd::StandardDatabase::GetSeriesQueryFromPatterns(std::vector<std::string> patterns)
{
  typedef odb::query<Serie> QueryType;
  QueryType q = (QueryType::id != 0); // required initialization.
  for(auto i=patterns.begin(); i<patterns.end(); i++)
    AndSeriesQueryFromPattern(q, *i);
  return q;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::AndSeriesQueryFromPattern(odb::query<Serie> & q, std::string pattern)
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
std::string syd::StandardDatabase::Print(Patient patient, int level)
{
  std::stringstream ss;
  ss << patient.name << " "
     << patient.study_id << "\t"
     << patient.weight_in_kg << " kg\t"
     << patient.injection_date;
  if (level > 0) {
    ss << "\t"  << (patient.was_treated ? "90Y":"no ") << "\t"
       << patient.injected_activity_in_MBq << " MBq";
  }
  if (level > 1) {
    // Get the number of series for this patient
    std::vector<Serie> series;
    LoadVector<Serie>(odb::query<Serie>::patient_id_fk == patient.id, series);
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
std::string syd::StandardDatabase::Print(Serie serie)
{
  Patient patient = GetById<Patient>(serie.patient_id_fk);
  std::stringstream ss;
  std::cout << patient.name << " "
            << patient.study_id << " "
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
void syd::StandardDatabase::CheckIntegrity(std::vector<std::string> & args)
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
void syd::StandardDatabase::CheckIntegrity(const Patient & patient)
{
  // Part 1
  ELOG(0) << "Part 1: from series to files";
  std::vector<Serie> series;
  LoadVector<Serie>(odb::query<Serie>::patient_id_fk == patient.id, series);
  ELOG(1) << "Found " << series.size() << " series. Checking ...";
  for(auto i=series.begin(); i<series.end(); i++) {
    ELOG(2) << "Checking serie " << i->id << " " << i->path;
    CheckSerie(*i);
  }

  // Part 2
  ELOG(0) << "Part 2 : from files to series (could be long)";
  // Search for all folders in patient folder
  std::string folder = GetPath(patient);

  // For all folders, find dicom
  if (!syd::DirExists(folder)) {
    LOG(FATAL) << "The folder " << folder << " does not exist.";
  }
  OFList<OFString> inputFiles;
  inputFiles.clear();
  OFString scanPattern = "*dcm";
  OFString dirPrefix = "";
  OFBool recurse = OFTrue;
  size_t found =
    OFStandard::searchDirectoryRecursively(folder.c_str(), inputFiles, scanPattern, dirPrefix, recurse);
  ELOG(1) << "Found " << inputFiles.size() << " files, checking...";

  // for all dicom check in db
  int n=0;
  int m = inputFiles.size()/10;
  for(auto i=inputFiles.begin(); i != inputFiles.end(); i++) {
    syd::loadbar(n, m); // FIXME
    CheckFile(*i);
  }
  ELOG(0) << "Done, everything seems correct.";

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::CheckFile(OFString filename)
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
    ELOG(0) << "CheckIntegrity : Error the file " << filename << " contains a dicom which is not in the db.";
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiType syd::StandardDatabase::GetRoiType(const std::string & name)
{
  std::vector<RoiType> roitypes;
  LoadVector<RoiType>(odb::query<RoiType>::name == name, roitypes);
  if (roitypes.size() != 1) {
    LOG(FATAL) << "Error while searching roi type named " << name
               << " : I found " << roitypes.size()
               << " roitypes(s) while expecting a single one";
  }
  return roitypes[0];
}
// --------------------------------------------------------------------


*/