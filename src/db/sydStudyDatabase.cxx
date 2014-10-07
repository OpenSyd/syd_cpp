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
void syd::StudyDatabase::CreateDatabase()
{
  DD("CreateDatabase");

  // DD("here");
  // odb::transaction t (db_->begin());
  // odb::schema_catalog::create_schema (*db_);
  // t.commit();

  DD("TODO");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::CheckIntegrity(std::vector<std::string> & args)
{
  std::string cmd;
  if (args.size() == 0) {
    cmd = "patient";
  }
  else {
    cmd = args[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
  }

  if ((cmd != "patient") &&
      (cmd != "rawimage") &&
      (cmd != "timepoint")) {
    LOG(FATAL) << "Please provide 'patient' or 'rawimage' or 'timepoint'";
  }

  // check rawimage
  if (cmd == "rawimage") {
    std::vector<RawImage> rawimages;
    if (args.size() == 1) {
      LoadVector<RawImage>(rawimages);
    }
    else {
      for(auto i=1; i<args.size(); i++) {
        int id = atoi(args[i].c_str());
        rawimages.push_back(GetById<RawImage>(id));
      }
    }
    for(auto i:rawimages) CheckIntegrity(i);
  }

  if (cmd == "timepoint") {
    std::vector<Timepoint> timepoints;
    if (args.size() == 1) {
      LoadVector<Timepoint>(timepoints);
    }
    else {
      for(auto i=1; i<args.size(); i++) {
        int id = atoi(args[i].c_str());
        timepoints.push_back(GetById<Timepoint>(id));
      }
    }
    for(auto i:timepoints) CheckIntegrity(i);
  }

  if (cmd == "patient") {
    std::vector<Patient> patients;
    if (args.size() == 1) {
      cdb_->LoadVector<Patient>(patients);
    }
    else {
      for(auto i=1; i<args.size(); i++) {
        std::string name = args[i].c_str();
        Patient patient;
        bool b = cdb_->GetIfExist<Patient>(odb::query<Patient>::name == name, patient);
        if (b) {
          patients.push_back(patient);
        }
        else {
          LOG(WARNING) << "Could not find patient " << name;
        }
      }
    }
    for(auto i:patients) CheckIntegrity(i);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::CheckIntegrity(const Patient & patient)
{
  // Get all timepoints for this patient
  std::vector<Timepoint> timepoints;
  LoadVector<Timepoint>(timepoints, odb::query<Timepoint>::patient_id == patient.id);

  // Order the  indices
  std::vector<size_t> indices;
  for(auto i=0; i != timepoints.size(); i++) indices.push_back(i);
  std::sort(begin(indices), end(indices),
            [&timepoints](size_t a, size_t b) { return timepoints[a].time_from_injection_in_hours < timepoints[b].time_from_injection_in_hours; }  );

  for(auto i=0; i<indices.size(); i++) {
    if (indices[i]+1 != timepoints[i].number) {
      LOG(WARNING) << "Error in timepoints numbering. Timepoint id=" << timepoints[i].id
                   << " has number " << timepoints[i].number << " while indice " << indices[i]+1;
    }
  }

  // Loop over all timepoints
  for(auto i:timepoints) CheckIntegrity(i);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::CheckIntegrity(const Timepoint & timepoint)
{
  // check p id exist
  Patient patient(GetPatient(timepoint));
  VLOG(1) << "Timepoint " << timepoint.id << " for patient " << patient.name;

  // check spect and ct serie id exist -> CheckIntegrity(serie) ?
  Serie spect_serie = cdb_->GetById<Serie>(timepoint.spect_serie_id);
  VLOG(1) << "With spect serie " << spect_serie.id << " " << spect_serie.path;
  double d = DateDifferenceInHours(spect_serie.acquisition_date, patient.injection_date);
  if (d != timepoint.time_from_injection_in_hours) {
    LOG(WARNING) << "Error in the time_from_injection_in_hours : I found " << timepoint.time_from_injection_in_hours
                 << " while it should be "  << d << std::endl
                 << "\t injection date is = " << patient.injection_date << std::endl
                 << "\t acquisition date is = " << spect_serie.acquisition_date;
  }

  // check time_from_injection_in_hours
  Serie ct_serie = cdb_->GetById<Serie>(timepoint.ct_serie_id);
  VLOG(1) << "With ct serie " << ct_serie.id << " " << ct_serie.path;

  // raw image exist
  RawImage spect(GetById<RawImage>(timepoint.spect_image_id));
  CheckIntegrity(spect);

  RawImage ct(GetById<RawImage>(timepoint.ct_image_id));
  CheckIntegrity(ct);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::CheckIntegrity(const RawImage & image)
{
  // check if file (mhd) exist
  std::string path = GetImagePath(image);
  if (!syd::FileExists(path)) {
    LOG(FATAL) << "Error in the db ('" << get_name() << "'), the file " << path << " do not exist.";
  }

  // check if file md5 is the same
  std::string m;
  if (image.pixel_type == "float") {
    typedef float PixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    ImageType::Pointer im = ReadImage<ImageType>(path);
    m = ComputeImageMD5<ImageType>(im);
  }
  if (image.pixel_type == "short") {
    typedef signed short PixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    ImageType::Pointer im = ReadImage<ImageType>(path);
    m = ComputeImageMD5<ImageType>(im);
  }
  if (m != image.md5) {
    LOG(WARNING) << "*Error* md5 for image " << image.id << " " << path;
  }
  else {
    VLOG(1) << "Correct md5 for image " << image.id << " " << path;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::InsertTimepoint(Timepoint & t, RawImage & spect, RawImage & ct)
{
  // Also create folder if needed
  Patient patient (GetPatient(t));
  std::string p = GetOrCreatePath(patient);
  // Insert all elements and update id
  spect.md5 = "";
  spect.pixel_type = "float";
  spect.path = p;
  ct.md5 = "";
  ct.pixel_type = "short";
  ct.path = p;
  Insert(spect);
  Insert(ct);
  t.spect_image_id = spect.id;
  t.ct_image_id = ct.id;
  Insert(t);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::CopyFilesFrom(std::shared_ptr<StudyDatabase> in_db,
                                       const Timepoint & in,
                                       Timepoint & out)
{
  // SPECT part
  RawImage in_spect(in_db->GetById<RawImage>(in.spect_image_id));
  RawImage out_spect(GetById<RawImage>(out.spect_image_id));
  if (in_spect.md5 != out_spect.md5) {
    std::string from(in_db->GetImagePath(in_spect));
    std::string to(GetImagePath(out_spect));
    VLOG(3) << "Copy image " << from << " to " << to;
    syd::CopyMHDImage(from, to);
    out_spect.md5 = in_spect.md5;
    Update(out_spect);
  }
  else {
    VLOG(3) << "Not copying files because same md5 for " << in_spect << " and " << out_spect;
  }

  // CT part
  RawImage in_ct(in_db->GetById<RawImage>(in.ct_image_id));
  RawImage out_ct(GetById<RawImage>(out.ct_image_id));
  if (in_ct.md5 != out_ct.md5) {
    std::string from(in_db->GetImagePath(in_ct));
    std::string to(GetImagePath(out_ct));
    VLOG(3) << "Copy image " << from << " to " << to;
    syd::CopyMHDImage(from, to);
    out_ct.md5 = in_ct.md5;
    Update(out_ct);
  }
  else {
    VLOG(3) << "Not copying files because same md5 for " << in_ct << " and " << out_ct;
  }
}
// --------------------------------------------------------------------


bool syd::StudyDatabase::FilesExist(Timepoint t)
{
  DD("TODO");
}
bool syd::StudyDatabase::CheckMD5(Timepoint t)
{
  DD("TODO");
}

// --------------------------------------------------------------------
void syd::StudyDatabase::UpdatePathAndRename(const Timepoint & timepoint, bool rename_flag)
{
  Patient patient(GetPatient(timepoint));

  // Build filename and path for spect
  RawImage spect(GetById<RawImage>(timepoint.spect_image_id));
  std::string from = GetImagePath(spect);
  spect.path = patient.name+PATH_SEPARATOR;
  spect.filename = "spect"+toString(timepoint.number)+".mhd";
  std::string to = GetImagePath(spect);
  if (rename_flag) syd::RenameMHDImage(from, to, 2);

  // Build filename and path for ct
  RawImage ct(GetById<RawImage>(timepoint.ct_image_id));
  from = GetImagePath(ct);
  ct.path = patient.name+PATH_SEPARATOR;
  ct.filename = "ct"+toString(timepoint.number)+".mhd";
  to = GetImagePath(ct);
  if (rename_flag) syd::RenameMHDImage(from, to, 2);

  Update(spect);
  Update(ct);
}
// --------------------------------------------------------------------


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
    series.push_back(cdb_->GetById<Serie>(i->spect_serie_id));
  }

  // Order the  indices
  std::vector<size_t> indices;
  for(auto i=0; i != series.size(); i++) indices.push_back(i);
  std::sort(begin(indices), end(indices),
            [&series](size_t a, size_t b) { return syd::IsBefore(series[a].acquisition_date, series[b].acquisition_date); }  );


  // Change the numbers
  for(auto i=0; i<timepoints.size(); i++) { // two loops needed
    Timepoint & t = timepoints[indices[i]]; // (the & is very important ! If not = copy)
    t.number = i+1;
  }

  // Rename the associated files
  // 1) rename all temporary  (to avoid overwrite)
  // 2) rename
  for(auto i:timepoints) {
    // spect
    RawImage spect(GetById<RawImage>(i.spect_image_id));
    std::string from = GetImagePath(spect);
    spect.filename = spect.filename+"TMP.mhd";
    std::string to = GetImagePath(spect);
    syd::RenameMHDImage(from, to, 2);
    Update(spect);
    // ct
    RawImage ct(GetById<RawImage>(i.ct_image_id));
    from = GetImagePath(ct);
    ct.filename = ct.filename+"TMP.mhd";
    to = GetImagePath(ct);
    syd::RenameMHDImage(from, to, 2);
    Update(ct);
  }

  for(auto i:timepoints) {
    UpdatePathAndRename(i);
  }

  odb::transaction t (db_->begin());
  for(auto i=0; i<timepoints.size(); i++) {
    db_->update(timepoints[i]);
  }
  t.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::ConvertDicomToImage(const Timepoint & t)
{
  // Spect part
  RawImage spect(GetById<RawImage>(t.spect_image_id));
  std::string dicom_filename = cdb_->GetSeriePath(t.spect_serie_id); // spect.serie_id
  std::string mhd_filename = GetImagePath(spect);
  VLOG(2) << "Converting SPECT dicom to mhd (" << mhd_filename << ") ...";
  spect.md5 = syd::ConvertDicomSPECTFileToImage(dicom_filename, mhd_filename);
  Update(spect);

  // CT part
  RawImage ct = GetById<RawImage>(t.ct_image_id);
  std::string dicom_path = cdb_->GetSeriePath(t.ct_serie_id);
  std::string ct_mhd_filename = GetImagePath(ct);
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
std::string syd::StudyDatabase::GetRoiPath(const Patient & p)
{
  return GetPath(p)+PATH_SEPARATOR+"roi"+PATH_SEPARATOR;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StudyDatabase::GetOrCreatePath(const Patient & p)
{
  std::string path = GetPath(p);
  if (!syd::DirExists(path)) {
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
std::string syd::StudyDatabase::GetOrCreateRoiPath(const Patient & p)
{
  GetOrCreatePath(p);
  std::string path = GetRoiPath(p);
  if (!syd::DirExists(path)) {
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
std::string syd::StudyDatabase::GetImagePath(const RawImage & image)
{
  return get_folder()+PATH_SEPARATOR+image.path+PATH_SEPARATOR+image.filename;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StudyDatabase::GetRegistrationOutputPath(Timepoint ref, Timepoint mov)
{
  Patient p(GetPatient(ref));
  std::string path = GetPath(p)+PATH_SEPARATOR+"output";
  if (!syd::DirExists(path)) {
    VLOG(3) << "Creating folder " << path;
    int ret = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (ret != 0) {
      LOG(FATAL) << "Error while attempting to create " << path;
    }
  }
  path = path+PATH_SEPARATOR+"ct"+toString(ref.number)+"-ct"+toString(mov.number);
  if (!syd::DirExists(path)) {
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
  Serie serie = cdb_->GetById<Serie>(t.spect_serie_id);
  Patient patient(GetPatient(t));
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


// --------------------------------------------------------------------
Patient syd::StudyDatabase::GetPatient(const Timepoint & timepoint)
{
  return cdb_->GetById<Patient>(timepoint.patient_id);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage syd::StudyDatabase::GetRoiMaskImage(const Timepoint & timepoint, std::string roiname)
{
  RoiType roitype(cdb_->GetRoiType(roiname));
  Patient patient(GetPatient(timepoint));
  std::vector<RoiMaskImage> roimaskimages;
  LoadVector<RoiMaskImage>(roimaskimages,
                           odb::query<RoiMaskImage>::timepoint_id == timepoint.id &&
                           odb::query<RoiMaskImage>::roitype_id == roitype.id);
  if (roimaskimages.size() != 1) {
    LOG(FATAL) << "Error while searching roi '" <<  roiname << "' associated with timepoint " << timepoint.number
               << " of " << patient.name << " : I found "
               << roimaskimages.size() << " roi(s) while expecting a single one";
   }
  return roimaskimages[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::InsertRoiMaskImage(const Timepoint & timepoint,
                                            const RoiType & roitype,
                                            RoiMaskImage & roi)
{
  // Create new RawImage associated with the roi
  RawImage mask;
  Insert(mask);

  // Insert roi
  roi.mask_id = mask.id;
  roi.timepoint_id = timepoint.id;
  roi.roitype_id = roitype.id;
  Insert(roi);

  UpdateRoiMaskImage(roi);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::UpdateRoiMaskImage(RoiMaskImage & roi)
{
  // Get RawImage associated with the roi
  RawImage mask (GetById<RawImage>(roi.mask_id));
  Timepoint timepoint(GetById<Timepoint>(roi.timepoint_id));
  RoiType roitype(cdb_->GetById<RoiType>(roi.roitype_id));
  Patient patient(GetPatient(timepoint));

  mask.filename = roitype.name+toString(timepoint.number)+".mhd";
  mask.md5 = "";
  mask.path = patient.name+PATH_SEPARATOR+std::string("roi")+PATH_SEPARATOR;
  mask.pixel_type = "uchar";
  Update(mask);

  // Update roi
  roi.volume_in_cc = 0.0;
  Update(roi);

  // Create path if needed
  std::string path = GetRoiPath(patient);
  if (!syd::DirExists(path)) {
    VLOG(3) << "Creating folder " << path;
    int ret = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (ret != 0) {
      LOG(FATAL) << "Error while attempting to create " << path;
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::UpdateMD5(RawImage & image)
{
  std::string path = GetImagePath(image);
  std::string m = "unknown pixel type?";
  if (image.pixel_type == "short") {
    typedef itk::Image<signed short, 3> ImageType;
    ImageType::Pointer im = ReadImage<ImageType>(path);
    m = ComputeImageMD5<ImageType>(im);
  }
  if (image.pixel_type == "float") {
     typedef itk::Image<float, 3> ImageType;
     ImageType::Pointer im = ReadImage<ImageType>(path);
     m = ComputeImageMD5<ImageType>(im);
  }
  if (image.pixel_type == "uchar") {
     typedef itk::Image<unsigned char, 3> ImageType;
     ImageType::Pointer im = ReadImage<ImageType>(path);
     m = ComputeImageMD5<ImageType>(im);
  }

  if (m != image.md5) {
    VLOG(2) << "Updating image " << image.filename;
    image.md5 = m;
    Update(image);
  }
  else {
    VLOG(2) << "Image " << path << " already up-to-date";
  }
}
// --------------------------------------------------------------------
