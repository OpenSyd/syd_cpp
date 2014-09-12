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
void syd::InsertDicomCommand::SetArgs(char ** inputs, int n)
{
  DD("TODO");
  if (n != 2) {
    LOG(FATAL) << "Two parameter are needed <patient> <foldername> (you provide " << n << ").";
  }
  patient_name_ = inputs[0];
  folder_ = inputs[1];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertDicomCommand::Run()
{
  // Check database
  if (databases_.size() != 1) {
    LOG(FATAL) << "A (single) database of type ClinicalTrialDatabase "
               << "is needed in InsertDicomCommand. Aborting.";
  }
  db = static_cast<ClinicalTrialDatabase*>(databases_[0]);

  // Get or create the new patient
  Patient patient;
  if (db->GetIfExist<Patient>(odb::query<Patient>::name == patient_name_, patient)) {
    db->CheckPatient(patient);
  }
  else {
    VLOG(0) << "Create new patient " << patient_name_;
    db->AddPatient(patient_name_, patient);
  }

  // Search for all the files in the directory
  VLOG(0) << "Search for Dicom (*.dcm) in " << folder_;
  OFList<OFString> inputFiles;
  OFString scanPattern = "*dcm";
  OFString dirPrefix = "";
  if (folder_.empty()) {
    LOG(FATAL) << "The 'folder' is void. Abort.";
  }
  if (folder_.at(0) != PATH_SEPARATOR)  { // the folder is not an absolute path
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))  {
      LOG(FATAL) << "Error while trying to get current working dir.";
    }
    folder_ = std::string(cCurrentPath)+"/"+folder_;
  }
  OFBool recurse = OFTrue;
  size_t found=0;
  if (OFStandard::dirExists(folder_.c_str())) {
    found = OFStandard::searchDirectoryRecursively(folder_.c_str(), inputFiles, scanPattern, dirPrefix, recurse);
  }
  else {
    LOG(FATAL) << "The directory " << folder_ << " does not exist.";
  }
  if (inputFiles.size() > 0) {
    VLOG(0) << "Found " << inputFiles.size() << " files. Now parsing dicom ...";
  }
  else {
    VLOG(0) << "No files found.";
    return;
  }

  // Loop on the files
  const char * filename = NULL;
  OFListIterator(OFString) if_iter = inputFiles.begin();
  OFListIterator(OFString) if_last = inputFiles.end();
  DcmTagKey key = GetTagKey("SeriesInstanceUID");
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

    std::string v = GetTagValue(dset, key);
    if (map_series_.find(v) != map_series_.end()) { // already exist
      map_series_[v].filenames_.push_back(filename);
    }
    else { // new serie found
      DicomSerieInfo s;
      s.serie_UID_ = v;
      s.filenames_.push_back(filename);
      map_series_[v] = s;
    }
  }
  VLOG(0) << "Found " << map_series_.size() << " series. Now importing into the db..." ;

  // For all series, update the DB
  for(auto i=map_series_.begin(); i!=map_series_.end(); i++) {
    UpdateDicom(patient, i->second);
  }
  VLOG(0) << "Done. " << inputFiles.size() << " were updated in the db.";
}

// --------------------------------------------------------------------
void syd::InsertDicomCommand::UpdateDicom(Patient & patient, const DicomSerieInfo & d)
{
  // Read file header
  DcmFileFormat dfile;
  DcmObject *dset = &dfile;
  dset = dfile.getDataset();
  dfile.loadFile(d.filenames_[0].c_str());
  DD(patient);

  // Find the patient ID
  std::string PatientName = GetTagValue(dset, "PatientName");
  std::string PatientDicomId = GetTagValue(dset, "PatientID");
  VLOG(1) << "Found Patient " << PatientName << " " << PatientDicomId;

  // TimePoint
  std::string StudyTime = GetTagValue(dset, "StudyTime");
  std::string StudyDate = GetTagValue(dset, "StudyDate");
  DD(StudyTime);
  DD(StudyDate);

  std::string SeriesTime = GetTagValue(dset, "SeriesTime");
  std::string SeriesDate = GetTagValue(dset, "SeriesDate");
  DD(SeriesTime);
  DD(SeriesDate);

  // Serie
  std::string SeriesDescription = GetTagValue(dset, "SeriesDescription");
  std::string SeriesInstanceUID = GetTagValue(dset, "SeriesInstanceUID");
  VLOG(1) << "Serie " << SeriesInstanceUID;

  // Create the series
  Serie serie;
  DD(serie);
  if (db->GetIfExist<Serie>(odb::query<Serie>::dicom_uid == SeriesInstanceUID, serie)) {
    // already exist
    db->CheckSerie(serie);
    LOG(WARNING) << "Serie " << SeriesInstanceUID << " already exist, updating.";
  }
  else db->AddSerie(patient, SeriesDescription, SeriesInstanceUID, serie);
  DD(serie);


  // Update the fields
  //  serie.acui

  /* What to do with the series ?
     --> Patient : create new folder (or already exist) = dicom (name + ID)
     --> TimePoint : create new folder (or already exist)
         update time_from_injection_in_hours
         update time_number
     --> Serie : create new folder (or already exist) with serieid (or description ?)

     SERIEUID ???

         = Update(list of files)
         Check modality CT or SPECT.
         CT : create ct dicom folder + copy    --> if exist remove first or ask ?

         Series Instance UID


         SPECT
         create spect dicom folder + copy --> if exist remove first or ask ?
         update serie_description


         SPECT
         20120113 125546 SAVE_SPECT_KNITTED
         (0020,000d) ?? (UI) [1.3.46.670589.28.1.1.148673388.2980821302365]         # 44,1 Study Instance UID
         ==> (0008,1140) ?? (SQ)                                               # u/l,1 Referenced Image Sequence
         (0020,000e) ?? (UI) [1.3.46.670589.5.2.10.2.4.46.678.1.9272.1326470117187]         # 52,1 Series Instance UID
         (0020,000d) ?? (UI) [1.3.46.670589.28.1.1.148673388.2980821302365]         # 44,1 Study Instance UID
         (0020,000e) ?? (UI) [1.3.46.670589.5.2.10.2.4.46.678.1.9272.1326470117015.45413770]         # 62,1 Series Instance UID
         (0020,0052) ?? (UI) [1.3.46.670589.28.1.1.148673388.1326457423.502.2980821302365]         # 60,1 Frame of Reference UID


         CT 20120113 125546 SAVE_CT_KNITTED
         (0020,000d) ?? (UI) [1.3.46.670589.28.1.1.148673388.2980821302365]         # 44,1 Study Instance UID
         (0020,000e) ?? (UI) [1.3.46.670589.5.2.10.2.4.46.678.1.9272.1326470117187]         # 52,1 Series Instance UID
         (0020,0052) ?? (UI) [1.3.46.670589.28.1.1.148673388.1326457423.502.2980821302365]         # 60,1 Frame of Reference UID

         CT 20120113 125546 LOC_XCT_-_1_
         (0020,000d) ?? (UI) [1.3.46.670589.28.1.1.148673388.2980821302365]         # 44,1 Study Instance UID
         (0020,000e) ?? (UI) [1.3.46.670589.28.1.1.148673388.1326457423.503.2980821302365]         # 60,1 Series Instance UID
         (0020,0052) ?? (UI) [1.3.46.670589.28.1.1.148673388.1326457423.502.2980821302365]         # 60,1 Frame of Reference UID

         CT 20120111 130141 SAVE_CT_KNITTED
         (0020,000d) ?? (UI) [1.3.46.670589.28.1.1.148673388.2980700377783]         # 44,1 Study Instance UID
         (0020,000e) ?? (UI) [1.3.46.670589.5.2.10.2.4.46.678.1.10996.1326294040328]         # 54,1 Series Instance UID
         (0020,0052) ?? (UI) [1.3.46.670589.28.1.1.148673388.1326286676.372.2980700377783]         # 60,1 Frame of Reference UID

         Study Instance UID -> "date"
         in SPECT embedded Series Instance UID of the CT


  */
}
// --------------------------------------------------------------------
