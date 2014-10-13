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
#include "sydInsertDicomCommand.h"

// --------------------------------------------------------------------
syd::InsertDicomCommand::InsertDicomCommand(std::string db):DatabaseCommand()
{
  cdb_ = syd::Database::OpenDatabaseType<ClinicDatabase>(db);
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertDicomCommand::InsertDicomCommand(syd::ClinicDatabase * db):
  cdb_(db)
{
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertDicomCommand::Initialization()
{
  patient_name_ = "noname";
  set_rename_flag(false);

  // We need to add the DatasetName tag to the dicom dictionary
  DcmDictEntry * e = new DcmDictEntry(0x0011, 0x1012, EVR_LO, "DatasetName", 0, DcmVariableVM, NULL, true, NULL);
  DcmDataDictionary &globalDataDict = dcmDataDict.wrlock();
  globalDataDict.addEntry(e);

  e = new DcmDictEntry(0x0020, 0x0013, EVR_IS, "InstanceNumber", 0, DcmVariableVM, NULL, true, NULL);
  globalDataDict.addEntry(e);

  // try to remove warning from dcmtk (dont work)
  //dcmtk::log4cplus::helpers::LogLog * loglog = dcmtk::log4cplus::helpers::LogLog::getLogLog();
  //loglog->setQuietMode(true);
  //loglog->setInternalDebugging(false);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertDicomCommand::~InsertDicomCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertDicomCommand::InsertDicom(std::string patient_name, std::vector<std::string> & folders)
{
  // Loop on given folders
  for(auto i:folders) InsertDicom(patient_name, i);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertDicomCommand::InsertDicom(std::string patient_name, std::string folder)
{
  // Get patient
  Patient patient;
  bool b = cdb_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name, patient);
  if (!b) {
    LOG(FATAL) << "Error I could not find the patient '" << patient_name << "' in the db.";
  }

  // Create folder if needed
  std::string path = cdb_->GetPath(patient);
  if (!syd::DirExists(path)) syd::CreateDirectory(path);

  // Search for all the files in the directory
  OFList<OFString> inputFiles;
  inputFiles.clear();
  OFString scanPattern = "*dcm";
  OFString dirPrefix = "";
  if (folder.empty()) {
    LOG(FATAL) << "The 'foldername' is void. Abort.";
  }
  if (folder.at(0) != PATH_SEPARATOR)  { // the folder is not an absolute path
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))  {
      LOG(FATAL) << "Error while trying to get current working dir.";
    }
    folder = std::string(cCurrentPath)+"/"+folder;
    //dirPrefix = OFString(cCurrentPath);
  }
  VLOG(0) << "Search for Dicom (*.dcm) in " << folder;
  OFBool recurse = OFTrue;
  size_t found=0;
  if (syd::DirExists(folder)) {
    found = OFStandard::searchDirectoryRecursively(folder.c_str(), inputFiles, scanPattern, dirPrefix, recurse);
  }
  else {
    LOG(WARNING) << "The directory " << folder << " does not exist.";
    return;
  }
  // Debug : inputFiles.push_back(*inputFiles.begin());
  int n_before = inputFiles.size();

  // I dont know why but sometimes, the recursive search find the same
  // files several times. Here is a workaournd to remove duplicate
  // files. To remove duplicate : conversion to set.
  std::vector<OFString> v;
  for(auto i=inputFiles.begin(); i!=inputFiles.end(); i++) v.push_back(*i);
  std::set<OFString> s (v.begin(), v.end() );
  v.assign( s.begin(), s.end() );
  inputFiles.clear();
  for(auto i=v.begin(); i<v.end(); i++) inputFiles.push_back(*i);
  int n_after = inputFiles.size();
  if (n_before != n_after) {
    LOG(WARNING) << "Find duplicates files, I remove them.";
  }

  if (inputFiles.size() > 0) {
    VLOG(0) << "Found " << inputFiles.size() << " files. Now parsing dicom ...";
  }
  else {
    VLOG(0) << "No files found.";
    return;
  }

  // Loop on the files
  std::map<std::string, DicomSerieInfo> map_series;
  const char * filename = NULL;
  OFListIterator(OFString) if_iter = inputFiles.begin();
  OFListIterator(OFString) if_last = inputFiles.end();
  int n = inputFiles.size()/10;
  int i=0;
  while (if_iter != if_last) {
    filename = (*if_iter++).c_str();
    VLOG_EVERY_N(n, 1) << ++i << "/10";

    DcmFileFormat dfile;
    bool b = syd::OpenDicomFile(filename, true, dfile);
    if (!b) continue;
    DcmObject *dset = dfile.getDataset();

    std::string seriesUID = GetTagValueString(dset, "SeriesInstanceUID");
    std::string SOP_UID = GetTagValueString(dset, "SOPInstanceUID");
    std::string modality = GetTagValueString(dset, "Modality");
    std::string k;
    if (modality == "CT") k = seriesUID;
    else k = SOP_UID;

    if (map_series.find(k) != map_series.end()) { // already exist
      map_series[k].filenames_.push_back(filename);
    }

    else { // new serie found
      DicomSerieInfo s;
      s.serie_UID_ = k;
      s.filenames_.push_back(filename);
      map_series[k] = s;
    }
  }
  VLOG(0) << "Found " << map_series.size() << " series. Now importing into the db..." ;

  // For all series, update the DB
  for(auto i=map_series.begin(); i!=map_series.end(); i++) {
    UpdateDicom(patient, i->second);
  }
  VLOG(0) << "Done. " << map_series.size() << " series were updated in the db for patient " << patient.name << ".";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertDicomCommand::UpdateDicom(Patient & patient, const DicomSerieInfo & d)
{
  // Read file header
  DcmFileFormat dfile;
  bool b = syd::OpenDicomFile(d.filenames_[0], true, dfile);
  if (!b) {
    LOG(FATAL) << "Could not open dicom " << d.filenames_[0];
  }
  DcmObject *dset = dfile.getDataset();


  // Find the patient ID
  std::string PatientName = GetTagValueString(dset, "PatientName");
  std::string PatientDicomId = GetTagValueString(dset, "PatientID");
  VLOG(1) << "Found Patient " << PatientName << " " << PatientDicomId << " " << d.filenames_[0];

  // Sanity check
  int n = PatientName.find("^");
  if (n != std::string::npos) {
    std::string initials = PatientName[0] + PatientName.substr(n+1,1);
    std::transform(initials.begin(), initials.end(), initials.begin(), ::tolower);
    if (initials != patient.name) {
      LOG(FATAL) << "Error the patient in the dicom is " << PatientName
                 << " (" << initials << "), while you ask for "
                 << patient.name;
    }
  }

  // Modality
  std::string modality = GetTagValueString(dset, "Modality");
  if (modality != "CT") modality = "NM";

  // Serie
  std::string AcquisitionTime = GetTagValueString(dset, "AcquisitionTime");
  std::string AcquisitionDate = GetTagValueString(dset, "AcquisitionDate");
  std::string SeriesDescription = GetTagValueString(dset, "SeriesDescription");
  std::string StudyDescription = GetTagValueString(dset, "StudyDescription");
  std::string SeriesInstanceUID = GetTagValueString(dset, "SeriesInstanceUID");
  std::string SOPInstanceUID = GetTagValueString(dset, "SOPInstanceUID");
  std::string FrameOfReferenceUID = GetTagValueString(dset, "FrameOfReferenceUID");
  std::string ImageID = GetTagValueString(dset, "ImageID");
  std::string Manufacturer = GetTagValueString(dset, "Manufacturer");
  std::string ManufacturerModelName = GetTagValueString(dset, "ManufacturerModelName");
  std::string DatasetName = GetTagValueString(dset, "DatasetName");
  std::string TableTraverse = GetTagValueString(dset, "TableTraverse");
  std::string ContentDate = GetTagValueString(dset, "ContentDate");
  std::string ContentTime = GetTagValueString(dset, "ContentTime");
  std::string InstanceNumber = GetTagValueString(dset, "InstanceNumber");

  std::string rec_date = "unknown";
  if (ContentDate != "") {
    if (ContentTime == "")  ContentTime = "000000";
    rec_date = GetDate(ContentDate, ContentTime);
  }
  std::string acqui_date = "unknown";
  if (AcquisitionDate != "" && AcquisitionTime != "")
    acqui_date = GetDate(AcquisitionDate, AcquisitionTime);
  else {
    VLOG(0) << "Unknown acquisition date, ignoring the dicom "
            << d.filenames_[0].c_str();
    return;
  }

  std::string uid;
  if (modality == "CT") uid = SeriesInstanceUID;
  else uid = SOPInstanceUID;

  // Create the series
  Serie serie;
  if (cdb_->GetIfExist<Serie>(odb::query<Serie>::dicom_uid == uid, serie)) {
    // already exist
    // cdb_->CheckSerie(serie);
    VLOG(1) << "Serie id=" << serie.id << " at " << acqui_date << " already exist, updating.";
  }
  else {
    serie.path = "";
    serie.patient_id = patient.id;
    serie.dicom_uid = uid;
    cdb_->Insert(serie);
    VLOG(1) << "Create new serie=" << serie.id << " at " << acqui_date;
  }

  // Update the fields
  serie.dicom_uid = uid;
  serie.acquisition_date = acqui_date;
  serie.reconstruction_date = rec_date;
  serie.modality = modality;
  serie.dicom_frame_of_reference_uid = FrameOfReferenceUID;
  serie.dicom_dataset_name = DatasetName;
  serie.dicom_image_id = ImageID;
  serie.dicom_series_desc = SeriesDescription;
  serie.dicom_study_desc = StudyDescription;
  serie.dicom_manufacturer = Manufacturer;
  serie.dicom_manufacturer_model_name = ManufacturerModelName;
  serie.dicom_instance_number = InstanceNumber;
  serie.number_of_files = d.filenames_.size();

  // add to db (and create folder)
  cdb_->UpdateSerie(serie);

  // For CT modality, there are several files to copy
  if (modality == "CT") {
    for(auto i=d.filenames_.begin(); i<d.filenames_.end(); i++) {
      OFString filename;
      OFStandard::getFilenameFromPath(filename, i->c_str());
      std::string destination = cdb_->GetPath(serie)+PATH_SEPARATOR+filename.c_str();

      // Check if already exist
      if (syd::FileExists(destination)) {
        VLOG(3) << "File already exist, skip copying " << filename;
      }
      else {
        if (*i == destination) {
          VLOG(3) << "Same source/destination ignoring file " << filename;
        }
        else {
          VLOG(3) << "Copying " << filename;
          std::ifstream  src(i->c_str(), std::ios::binary);
          std::ofstream  dst(destination,   std::ios::binary);
          dst << src.rdbuf();
        }
      }
    }
  }
  else {
    if (d.filenames_.size() != 1) {
      LOG(WARNING) << "Error I found " << d.filenames_.size() << " files while expecting a single one for NM modality. I only consider the first one.";
    }
    std::string destination = cdb_->GetPath(serie);
    if (rename_flag_) {
      VLOG(2) << "Rename " << d.filenames_[0] << " to " << destination;
      if (syd::FileExists(destination)) {
        LOG(FATAL) << "Error the destination already exist : " << destination;
      }
      int result = rename(d.filenames_[0].c_str(), destination.c_str());
      if (result != 0) {
        LOG(FATAL) << "Error while renaming " << d.filenames_[0].c_str() << " to " << destination;
      }
    }
    else {
      if (d.filenames_[0] == destination) {
        VLOG(2) << "Same source/destination ignoring file " << destination;
      }
      else {
        VLOG(2) << "Copy " << d.filenames_[0] << " to " << destination;
        std::ifstream  src(d.filenames_[0].c_str(), std::ios::binary);
        std::ofstream  dst(destination, std::ios::binary);
        dst << src.rdbuf();
      }
    }
  }
}
// --------------------------------------------------------------------
