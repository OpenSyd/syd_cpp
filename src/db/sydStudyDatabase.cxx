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
#include "sydStudyDatabase.h"

// --------------------------------------------------------------------
syd::StudyDatabase::StudyDatabase(std::string name, std::string param):Database(name)
{
  // List all params
  std::istringstream f(param);
  SetFileAndFolder(f);
  std::string cdb_name;
  if (!getline(f, cdb_name, ';')) {
    LOG(FATAL) << "Error while parsing db cdb name for folder. db is "
               << name << " (" << get_typename() << ") params = " << param;
  }

  OpenSqliteDatabase(filename_, folder_);

  cdb_ = syd::Database::OpenDatabaseType<ClinicDatabase>(cdb_name);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::StudyDatabase::~StudyDatabase()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::InsertTimepoint(Timepoint & t, RawImage & spect, RawImage & ct)
{
  // Insert all elements and update id
  Insert(spect);
  Insert(ct);
  t.spect_image_id = spect.id;
  t.ct_image_id = ct.id;
  Insert(t);
  // Also create folder if needed
  Patient patient (cdb_->GetById<Patient>(t.patient_id));
  GetOrCreatePath(patient);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::CopyFilesTo(const Timepoint & in, std::shared_ptr<StudyDatabase> out_db, Timepoint & out)
{
  // SPECT part
  RawImage in_spect(GetById<RawImage>(in.spect_image_id));
  RawImage out_spect(out_db->GetById<RawImage>(out.spect_image_id));
  CopyFilesTo(in_spect, out_db, out_spect);
  // CT part
  RawImage in_ct(GetById<RawImage>(in.ct_image_id));
  RawImage out_ct(out_db->GetById<RawImage>(out.ct_image_id));
  CopyFilesTo(in_ct, out_db, out_ct);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::CopyFilesTo(const RawImage & in, std::shared_ptr<StudyDatabase> out_db, RawImage & out)
{
  // Check md5
  if (in.md5 != out.md5) {
    std::string from(GetPath(in));
    std::string to(out_db->GetPath(out));
    VLOG(3) << "Copy image " << from << " to " << to;
    syd::CopyMHDImage(from, to);
    out.md5 = in.md5;
  }
  else {
    VLOG(3) << "Not copying files because same md5 for " << in << " and " << out;
  }
}
// --------------------------------------------------------------------


// bool syd::StudyDatabase::HasSameFiles(Timepoint first, StudyDatabase * second_db, Timepoint second)
// {
//   DD("TODO");
// }

bool syd::StudyDatabase::FilesExist(Timepoint t)
{
  DD("TODO");
}
bool syd::StudyDatabase::CheckMD5(Timepoint t)
{
  DD("TODO");
}
void syd::StudyDatabase::UpdateMD5(Timepoint t)
{
  DD("TODO");
}


// --------------------------------------------------------------------
void syd::StudyDatabase::UpdateNumberAndRenameFiles(IdType patient_id)
{
  // Get the corresponding patient
  Patient patient = cdb_->GetById<Patient>(patient_id);

  // Get all timepoint for this patient
  std::vector<Timepoint> timepoints;
  LoadVector<Timepoint>(timepoints, odb::query<Timepoint>::patient_id == patient.id);

  // Get corresponding spect series acquisition_date
  std::vector<Serie> series;
  //std::vector<IdType> ids;
  odb::query<Serie> q =odb::query<Serie>(false);
  for(auto i=timepoints.begin(); i<timepoints.end(); i++) {
    RawImage spect = GetById<RawImage>(i->spect_image_id);
    series.push_back(cdb_->GetById<Serie>(spect.serie_id));
  }

  // Order the  indices
  std::vector<size_t> indices;
  for(auto i=0; i != series.size(); i++) indices.push_back(i);
  std::sort(begin(indices), end(indices),
            [&series](size_t a, size_t b) { return syd::IsBefore(series[a].acquisition_date, series[b].acquisition_date); }  );

  // Set the new order for all the timepoints
  std::vector<std::string> old_paths_spect(timepoints.size());
  std::vector<std::string> old_paths_ct(timepoints.size());
  for(auto i=0; i<timepoints.size(); i++) {
    std::string s = GetImagePath(timepoints[i].spect_image_id);
    old_paths_spect[i] = s;
    s = GetImagePath(timepoints[i].ct_image_id);
    old_paths_ct[i] = s;
  }

  for(auto i=0; i<timepoints.size(); i++) { // two loops needed
    Timepoint & t = timepoints[indices[i]]; // (the & is very important ! If not = copy)
    t.number = i+1;
    UpdateImageFilenames(t);
  }

  // Rename file (use a temporary filename to avoir overwriting the files)
  for(auto i=0; i<timepoints.size(); i++) {
    std::string spect = GetImagePath(timepoints[i].spect_image_id);
    if (old_paths_spect[i] != spect) {
      VLOG(3) << "Rename (old) " << old_paths_spect[i] << " to (new) " << spect;
      std::string path = spect+"TMP.mhd";
      syd::RenameMHDImage(old_paths_spect[i], path, 4); // 4 is verbose level
    }
    std::string ct = GetImagePath(timepoints[i].ct_image_id);
    if (old_paths_ct[i] != ct) {
      VLOG(3) << "Rename (old) " << old_paths_ct[i] << " to (new) " << ct;
      std::string path = ct+"TMP.mhd";
      syd::RenameMHDImage(old_paths_ct[i], path, 4);
    }
  }
  for(auto i=0; i<timepoints.size(); i++) {
    std::string spect = GetImagePath(timepoints[i].spect_image_id);
    if (old_paths_spect[i] != spect) {
      std::string pathTMP = spect+"TMP.mhd";
      syd::RenameMHDImage(pathTMP, spect, 4);
    }
    std::string ct = GetImagePath(timepoints[i].ct_image_id);
    if (old_paths_ct[i] != ct) {
      std::string pathTMP = ct+"TMP.mhd";
      syd::RenameMHDImage(pathTMP, ct, 4);
    }
  }

  // Update the DB (faster when doing like that)
  std::vector<RawImage> spects;
  std::vector<RawImage> cts;
  for(auto i=0; i<timepoints.size(); i++) {
    RawImage spect = GetById<RawImage>(timepoints[i].spect_image_id);
    RawImage ct = GetById<RawImage>(timepoints[i].ct_image_id);
    spects.push_back(spect);
    cts.push_back(ct);
  }

  odb::transaction t (db_->begin());
  for(auto i=0; i<timepoints.size(); i++) {
    db_->update(timepoints[i]);
    db_->update(spects[i]);
    db_->update(cts[i]);
  }
  t.commit();

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::ConvertDicomToImage(const Timepoint & t)
{
  // Spect part
  RawImage spect(GetById<RawImage>(t.spect_image_id));
  std::string dicom_filename = cdb_->GetSeriePath(spect.serie_id);
  std::string mhd_filename = GetImagePath(t.spect_image_id);
  VLOG(2) << "Converting SPECT dicom to mhd (" << mhd_filename << ") ...";
  spect.md5 = syd::ConvertDicomSPECTFileToImage(dicom_filename, mhd_filename);
  Update(spect);

  // CT part
  RawImage ct = GetById<RawImage>(t.ct_image_id);
  std::string dicom_path = cdb_->GetSeriePath(ct.serie_id);
  std::string ct_mhd_filename = GetImagePath(t.ct_image_id);
  ct.md5 = syd::ConvertDicomCTFolderToImage(dicom_path, ct_mhd_filename);
  Update(ct);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StudyDatabase::GetPath(const Patient & p)
{
  return get_folder()+PATH_SEPARATOR+p.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StudyDatabase::GetOrCreatePath(const Patient & p)
{
  std::string path = GetPath(p);
  if (!OFStandard::dirExists(path.c_str())) {
    VLOG(3) << "Creating folder " << path;
    int ret = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (ret != 0) {
      LOG(FATAL) << "Error while attempting to create " << path;
    }
  }
  return path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StudyDatabase::GetPath(const RawImage & image)
{
  Serie serie = cdb_->GetById<Serie>(image.serie_id);
  Patient patient = cdb_->GetById<Patient>(serie.patient_id);
  return GetPath(patient)+PATH_SEPARATOR+image.filename;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StudyDatabase::GetImagePath(IdType id)
{
  RawImage image = GetById<RawImage>(id);
  return GetPath(image);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::UpdateImageFilenames(const Timepoint & t)
{
  RawImage spect = GetById<RawImage>(t.spect_image_id);
  spect.filename = "spect"+toString(t.number)+".mhd";
  RawImage ct = GetById<RawImage>(t.ct_image_id);
  ct.filename = "ct"+toString(t.number)+".mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StudyDatabase::GetRegistrationOutputPath(Timepoint ref, Timepoint mov)
{
  Patient p(cdb_->GetById<Patient>(ref.patient_id));
  std::string path = GetPath(p)+PATH_SEPARATOR+"output";
  if (!OFStandard::dirExists(path.c_str())) {
    VLOG(3) << "Creating folder " << path;
    int ret = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (ret != 0) {
      LOG(FATAL) << "Error while attempting to create " << path;
    }
  }
  path = path+PATH_SEPARATOR+"ct"+toString(ref.number)+"-ct"+toString(mov.number);
  if (!OFStandard::dirExists(path.c_str())) {
    VLOG(3) << "Creating folder " << path;
    int ret = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (ret != 0) {
      LOG(FATAL) << "Error while attempting to create " << path;
    }
  }
  return path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StudyDatabase::Print(const Timepoint & t)
{
  RawImage spect = GetById<RawImage>(t.spect_image_id);
  Serie serie = cdb_->GetById<Serie>(spect.serie_id);
  Patient patient = cdb_->GetById<Patient>(serie.patient_id);
  std::stringstream ss;
  ss << patient.name << " " << t.id << " " << t.number << " " << serie.acquisition_date;
  return ss.str();
}
// --------------------------------------------------------------------


std::string syd::StudyDatabase::Print(const RawImage & t)
{
}


// --------------------------------------------------------------------
std::string syd::StudyDatabase::Print(const Patient & p, int level)
{
  // Print patient info
  std::stringstream ss;
  ss << cdb_->Print(p, level);
  // Get all associated timepoints
  typedef odb::query<Timepoint> QueryType;
  std::vector<Timepoint> timepoints;
  LoadVector<Timepoint>(timepoints, QueryType::patient_id == p.id);
  ss << "\t" << timepoints.size() << "\t";
  for(auto i=timepoints.begin(); i != timepoints.end(); i++) {
    ss << i->time_from_injection_in_hours << " ";
  }
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::Dump(std::ostream & os, std::vector<std::string> & args)
{
  // args
  if (args.size() > 1) {
    std::string s;
    for(auto i:args) s=s+i+" ";
    LOG(FATAL) << "Error, dump only require a single argument for a StudyDatabase, but you provide : "
               << s;
  }
  std::string patient_name;
  if (args.size() == 0) patient_name = "all";
  else patient_name = args[0];

  // Get the patients
  std::vector<Patient> patients;
  cdb_->GetPatientsByName(patient_name, patients);

  // Dump all patients
  for(auto i:patients) std::cout << Print(i) << std::endl;
};
// --------------------------------------------------------------------


/*
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------
std::string syd::StudyDatabase::GetFullPath(Patient patient)
{
  return get_folder()+patient.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StudyDatabase::GetFullPathSPECT(Timepoint timepoint)
{
  Serie serie = cdb_->GetById<Serie>(timepoint.serie_id);
  Patient patient = cdb_->GetById<Patient>(serie.patient_id);
  std::string p = GetFullPath(patient);
  return p+PATH_SEPARATOR+"spect"+toString(timepoint.number)+".mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StudyDatabase::GetFullPathCT(Timepoint timepoint)
{
  Serie serie = cdb_->GetById<Serie>(timepoint.serie_id);
  Patient patient = cdb_->GetById<Patient>(serie.patient_id);
  std::string p = GetFullPath(patient);
  return p+PATH_SEPARATOR+"ct"+toString(timepoint.number)+".mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::UpdateAllTimepointNumbers(IdType patient_id)
{
  // Get all timepoint for this patient
  std::vector<Timepoint> Study;
  LoadVector<Timepoint>(Study, odb::query<Timepoint>::patient_id == patient_id);

  // Get corresponding series acquisition_date
  std::vector<Serie> series;
  for(auto i=Study.begin(); i<Study.end(); i++) {
    series.push_back(cdb_->GetById<Serie>(i->serie_id));
  }

  // Ordered indices
  std::vector<size_t> indices;
  for(auto i=0; i != series.size(); i++) indices.push_back(i);
  std::sort(begin(indices), end(indices),
            [&series](size_t a, size_t b) { return syd::IsBefore(series[a].acquisition_date, series[b].acquisition_date); }  );

  // Set the new order for all the Study
  std::vector<std::string> old_paths_spect(Study.size());
  std::vector<std::string> old_paths_ct(Study.size());
  for(auto i=0; i<Study.size(); i++) {
    std::string s = GetFullPathSPECT(Study[i]);
    old_paths_spect[i] = s;
    s = GetFullPathCT(Study[i]);
    old_paths_ct[i] = s;
  }
  for(auto i=0; i<Study.size(); i++) { // two loops needed !
    Study[indices[i]].number = i+1;
  }

  // Rename file (use a temporary filename to avoir overwriting the files)
  for(auto i=0; i<Study.size(); i++) {
    if (old_paths_spect[i] != GetFullPathSPECT(Study[i])) {
      VLOG(3) << "Rename (old) " << old_paths_spect[i] << " to (new) " << GetFullPathSPECT(Study[i]);
      std::string path = GetFullPathSPECT(Study[i])+"TMP.mhd";
      syd::RenameMHDImage(old_paths_spect[i], path, 4); // 4 is verbose level
    }
    if (old_paths_ct[i] != GetFullPathCT(Study[i])) {
      VLOG(3) << "Rename (old) " << old_paths_ct[i] << " to (new) " << GetFullPathCT(Study[i]);
      std::string path = GetFullPathCT(Study[i])+"TMP.mhd";
      syd::RenameMHDImage(old_paths_ct[i], path, 4);
    }
  }
  for(auto i=0; i<Study.size(); i++) {
    if (old_paths_spect[i] != GetFullPathSPECT(Study[i])) {
      std::string pathTMP = GetFullPathSPECT(Study[i])+"TMP.mhd";
      std::string path = GetFullPathSPECT(Study[i]);
      syd::RenameMHDImage(pathTMP, path, 4);
    }
    if (old_paths_ct[i] != GetFullPathCT(Study[i])) {
      std::string pathTMP = GetFullPathCT(Study[i])+"TMP.mhd";
      std::string path = GetFullPathCT(Study[i]);
      syd::RenameMHDImage(pathTMP, path, 4);
    }
  }

  // Update the DB
  odb::transaction t (db->begin());
  for(auto i=0; i<Study.size(); i++) {
    db->update(Study[i]);
  }
  t.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StudyDatabase::Print(Timepoint t)
{
  Serie serie = cdb_->GetById<Serie>(t.serie_id);
  Patient patient = cdb_->GetById<Patient>(serie.patient_id);
  std::stringstream ss;
  ss << patient.name << " " << t.id << " " << t.number << " " << serie.acquisition_date;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StudyDatabase::Print(Patient p)
{
  typedef odb::query<Timepoint> QueryType;
  std::vector<Timepoint> Study;
  LoadVector<Timepoint>(Study, QueryType::patient_id == p.id);
  std::stringstream ss;
  ss << cdb_->Print(p, 1) << "\t"
     << Study.size() << "\t";
  for(auto i=Study.begin(); i != Study.end(); i++) {
    ss << i->time_from_injection_in_hours << " ";
  }
  ss << std::endl;

  return ss.str();
}
// --------------------------------------------------------------------
*/
