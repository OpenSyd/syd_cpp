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
syd::InsertDicomCommand::InsertDicomCommand():DatabaseCommand()
{
  db_ = NULL;
  patient_name_ = "noname";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertDicomCommand::~InsertDicomCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertDicomCommand::SetArgs(char ** inputs, int n)
{
  if (n < 2) {
    LOG(FATAL) << "Two parameters are needed <patient> <folders (could be multiple)>, but you provide "
               << n << " parameter(s)";
  }
  patient_name_ = inputs[0];
  for(auto i=1; i<n; i++)
    folders_.push_back(inputs[i]);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertDicomCommand::AddDatabase(syd::Database * d)
{
  if (databases_.size() != 0) {
    LOG(FATAL) << "InsertDicomCommand::AddDatabase: already a db.";
  }
  DatabaseCommand::AddDatabase(d);
  db_ = static_cast<ClinicalTrialDatabase*>(d);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertDicomCommand::Run()
{
  DD("run");
  // Check database
  if (db_ == NULL) {
    LOG(FATAL) << "A (single) database of type ClinicalTrialDatabase "
               << "is needed in InsertDicomCommand. Aborting.";
  }

  // Get the new patient
  if (db_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name_, patient_)) {
    db_->CheckPatient(patient_);
  }
  else {
    LOG(FATAL) << "Error, the patient " << patient_name_ << " does not exist";
  }

  // Loop on given folders
  for(auto i=0; i<folders_.size(); i++) Run(folders_[i]);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertDicomCommand::Run(std::string folder)
{
  // Search for all the files in the directory
  VLOG(0) << "Search for Dicom (*.dcm) in " << folder;
  OFList<OFString> inputFiles;
  OFString scanPattern = "*dcm";
  OFString dirPrefix = "";
  if (folder.empty()) {
    LOG(FATAL) << "The 'folder' is void. Abort.";
  }
  if (folder.at(0) != PATH_SEPARATOR)  { // the folder is not an absolute path
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))  {
      LOG(FATAL) << "Error while trying to get current working dir.";
    }
    folder = std::string(cCurrentPath)+"/"+folder;
  }
  OFBool recurse = OFTrue;
  size_t found=0;
  if (OFStandard::dirExists(folder.c_str())) {
    found = OFStandard::searchDirectoryRecursively(folder.c_str(), inputFiles, scanPattern, dirPrefix, recurse);
  }
  else {
    LOG(FATAL) << "The directory " << folder << " does not exist.";
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
  DcmTagKey seriesKey = GetTagKey("SeriesInstanceUID");
  DcmTagKey SOPKey = GetTagKey("SOPInstanceUID");
  DcmTagKey modalityKey = GetTagKey("Modality");
  int n = inputFiles.size()/10;
  int i=0;
  while (if_iter != if_last) {
    filename = (*if_iter++).c_str();
    VLOG_EVERY_N(n, 1) << ++i << "/10";

    DcmFileFormat dfile;
    DcmObject *dset = &dfile;
    dset = dfile.getDataset();

    const E_TransferSyntax xfer = EXS_Unknown; // auto detection
    const E_GrpLenEncoding groupLength = EGL_noChange;
    const E_FileReadMode readMode = ERM_autoDetect;
    const Uint32 maxReadLength = DCM_MaxReadLength;

    OFCondition cond = dfile.loadFile(filename, xfer, groupLength, maxReadLength, readMode);
    if (cond.bad())  {
      LOG(WARNING) << "Error : " << cond.text() << " while reading file "
                   << filename << " (not a Dicom ?)";
      continue;
    }

    std::string seriesUID = GetTagValue(dset, seriesKey); //FIXME
    std::string SOP_UID = GetTagValue(dset, SOPKey); //FIXME
    std::string modality = GetTagValue(dset, modalityKey); //FIXME
    std::string k;
    if (modality == "CT") {
      k = seriesUID;
    }
    else {
      k = SOP_UID;
    }

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
    UpdateDicom(patient_, i->second);
  }
  VLOG(0) << "Done. " << map_series.size() << " series were updated in the db.";
}

// --------------------------------------------------------------------
void syd::InsertDicomCommand::UpdateDicom(Patient & patient, const DicomSerieInfo & d)
{
  // Read file header
  DcmFileFormat dfile;
  DcmObject *dset = &dfile;
  dset = dfile.getDataset();
  dfile.loadFile(d.filenames_[0].c_str());

  // Find the patient ID
  std::string PatientName = GetTagValue(dset, "PatientName");
  std::string PatientDicomId = GetTagValue(dset, "PatientID");
  VLOG(1) << "Found Patient " << PatientName << " " << PatientDicomId << " " << d.filenames_[0];

  // Modality
  std::string modality = GetTagValue(dset, "Modality");
  if (modality != "CT") modality = "NM";

  // Serie
  std::string AcquisitionTime = GetTagValue(dset, "AcquisitionTime");
  std::string AcquisitionDate = GetTagValue(dset, "AcquisitionDate");
  std::string SeriesDescription = GetTagValue(dset, "SeriesDescription");
  std::string SeriesInstanceUID = GetTagValue(dset, "SeriesInstanceUID");
  std::string SOPInstanceUID = GetTagValue(dset, "SOPInstanceUID");
  std::string ImageID = GetTagValue(dset, "ImageID");
  std::string date = GetDate(AcquisitionDate, AcquisitionTime);

  std::string uid;
  if (modality == "CT") uid = SeriesInstanceUID;
  else uid = SOPInstanceUID;

  // Create the series
  Serie serie;
  if (db_->GetIfExist<Serie>(odb::query<Serie>::dicom_uid == uid, serie)) {
    // already exist
    db_->CheckSerie(serie);
    VLOG(1) << "Serie id=" << serie.id << " at " << serie.acquisition_date << " already exist, updating.";
  }
  else {
    serie.path = "";
    serie.patient_id = patient.id;
    serie.dicom_uid = uid;
    db_->Insert(serie);
    VLOG(1) << "Create new serie=" << serie.id << " at " << serie.acquisition_date;
  }

  // Update the fields
  serie.dicom_uid = uid;
  serie.acquisition_date = GetDate(AcquisitionDate, AcquisitionTime);
  serie.modality = modality;
  std::string desc;
  if (SeriesDescription == ImageID) desc = ImageID;
  else {
    if (ImageID.size() == 0) desc = SeriesDescription;
    else desc = SeriesDescription+"_"+ImageID;
  }
  std::replace(desc.begin(), desc.end(), ' ', '_');
  serie.dicom_description = desc;

  // add to db (and create folder)
  db_->UpdateSerie(serie);

  // Now copy the files
  VLOG(1) << "Copying files in the db folder " << db_->GetFullPath(serie);

  if (modality == "CT") {
    for(auto i=d.filenames_.begin(); i<d.filenames_.end(); i++) {
      OFString filename;
      OFStandard::getFilenameFromPath(filename, i->c_str());
      std::string destination = db_->GetFullPath(serie)+PATH_SEPARATOR+filename.c_str();

      // Check if already exist
      if (OFStandard::fileExists(destination.c_str())) {
        VLOG(2) << "File already exist, skip copying " << filename;
      }
      else {
        VLOG(2) << "Copying " << filename;
        std::ifstream  src(i->c_str(), std::ios::binary);
        std::ofstream  dst(destination,   std::ios::binary);
        dst << src.rdbuf();
      }
    }
  }
  else {
    if (d.filenames_.size() != 1) {
      LOG(FATAL) << "Error I found " << d.filenames_.size() << " files while expecting a single one for NM modality";
    }
    std::string destination = db_->GetFullPath(serie);
    std::ifstream  src(d.filenames_[0].c_str(), std::ios::binary);
    std::ofstream  dst(destination, std::ios::binary);
    dst << src.rdbuf();
    VLOG(2) << "Copy " << d.filenames_[0] << " to " << destination;
  }

}
// --------------------------------------------------------------------
