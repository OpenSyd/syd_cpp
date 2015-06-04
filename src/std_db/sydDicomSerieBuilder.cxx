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
#include "sydDicomSerieBuilder.h"
#include "sydTable.h"

// --------------------------------------------------------------------
syd::DicomSerieBuilder::DicomSerieBuilder(syd::StandardDatabase * db):syd::DicomSerieBuilder()
{
  SetDatabase(db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomSerieBuilder::DicomSerieBuilder()
{
  // Insert some tags that not always known in the dicom dictionary

  // DatasetName
  DcmDictEntry * e = new DcmDictEntry(0x0011, 0x1012, EVR_LO, "DatasetName", 0, DcmVariableVM, NULL, true, NULL);
  DcmDataDictionary &globalDataDict = dcmDataDict.wrlock();
  globalDataDict.addEntry(e);

  // InstanceNumber
  e = new DcmDictEntry(0x0020, 0x0013, EVR_IS, "InstanceNumber", 0, DcmVariableVM, NULL, true, NULL);
  globalDataDict.addEntry(e);

  // init
  patient_.id = -1;
  injection_.id = -1;
  db_ = NULL;
  useInjectionFlag_ = false;
  forcePatientFlag_ = false;
  forceUpdateFlag_ = false;
  nb_of_skip_files = 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomSerieBuilder::SetInjection(syd::Injection & injection)
{
  injection_ = injection;
  patient_ = *injection.patient;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomSerieBuilder::SearchForFilesInFolder(std::string folder, OFList<OFString> & inputFiles)
{
  // Search for all the files in the directory
  inputFiles.clear();
  std::string absolute_folder = folder;
  ConvertToAbsolutePath(absolute_folder);
  LOG(2) << "Search for files in " << folder;

  OFString scanPattern = "*"; // or *dcm ?
  OFString dirPrefix = "";
  OFBool recurse = OFTrue;
  size_t found=0;
  if (syd::DirExists(absolute_folder)) {
    found = OFStandard::searchDirectoryRecursively(absolute_folder.c_str(),
                                                   inputFiles, scanPattern,
                                                   dirPrefix, recurse);
  }
  else {
    LOG(FATAL) << "The directory " << absolute_folder << " does not exist.";
  }
  // I dont know why but sometimes, the recursive search find the same
  // files several times. Here is a workaournd to remove duplicate
  // files. To remove duplicate : conversion to set.
  int n_before = inputFiles.size();
  std::vector<OFString> v;
  for(auto i=inputFiles.begin(); i!=inputFiles.end(); i++) v.push_back(*i);
  std::set<OFString> s (v.begin(), v.end() );
  v.assign( s.begin(), s.end() );
  inputFiles.clear();
  for(auto i=v.begin(); i<v.end(); i++) inputFiles.push_back(*i);
  int n_after = inputFiles.size();
  if (n_before != n_after) {
    LOG(WARNING) << "Found duplicated files, I ignore them.";
  }

  if (inputFiles.size() > 0) {
    LOG(2) << "Found " << inputFiles.size() << " files. Now searching for dicom.";
  }
  else {
    LOG(WARNING) << "No files found.";
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomSerieBuilder::CreateDicomSerieFromFile(std::string filename)
{
  // Open the file
  DcmFileFormat dfile;
  bool b = syd::OpenDicomFile(filename, dfile);
  if (!b) {  // this is not a dicom file
    LOG(WARNING) << "Error the file '" << filename << "' is not a dicom file.";
    return;
  }
  DcmObject * dset = dfile.getAndRemoveDataset();

  // Test if this dicom file already exist in the db
  std::string sop_uid = GetTagValueString(dset, "SOPInstanceUID");
  if (DicomFileAlreadyExist(sop_uid)) {
    if (!forceUpdateFlag_) {
      LOG(2) << "Dicom file with same sop_uid already exist in the db. Skipping " << filename;
      nb_of_skip_files++;
      return;
    }
    syd::DicomFile f = db_->QueryOne<syd::DicomFile>(odb::query<syd::DicomFile>::dicom_sop_uid == sop_uid);
    LOG(2) << "Dicom already exist, we remove first both DicomSerie and DicomFile: " << f.file->filename;
    LOG(FATAL) << "On cascade not yet implemented. TODO.";
    //    db_->Delete(f);
  }

  // Test if a serie already exist in the database
  DicomSerie * serie;
  b = GuessDicomSerieForThisFile(filename, dset, &serie);

  // If this is a new DicomSerie, we create it
  if (!b) {
    serie = CreateDicomSerie(filename, dset);
    series_to_insert.push_back(serie);
    LOG(2) << "Creating a new serie: " << serie->dicom_series_uid;
  }

  // Then we add this dicomfile to the serie
  DicomFile * dicomfile = CreateDicomFile(filename, dset, serie);
  dicomfiles_to_insert.push_back(dicomfile);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::DicomSerieBuilder::DicomFileAlreadyExist(const std::string & sop_uid)
{
  int n = db_->Count<syd::DicomFile>(odb::query<syd::DicomFile>::dicom_sop_uid == sop_uid);
  if (n>0) return true;
  for(auto f:dicomfiles_to_insert) {
    if (f->dicom_sop_uid == sop_uid) return true;
  }
  return false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::DicomSerieBuilder::GuessDicomSerieForThisFile(const std::string & filename,
                                                        DcmObject * dset,
                                                        syd::DicomSerie ** serie)
{
  // Check in the future series
  std::string series_uid = GetTagValueString(dset, "SeriesInstanceUID");
  std::string modality = GetTagValueString(dset, "Modality");
  int index = -1;
  for(auto i=0; i<series_to_insert.size(); i++) {
    DicomSerie * s = series_to_insert[i];
    // First check series_uid and modality
    if (s->dicom_series_uid != series_uid) continue;
    if (modality != s->dicom_modality) continue;

    // Here we found a DicomSerie with the same series_uid
    // Very simple heuristic based on the modality
    if (s->dicom_modality == "CT") {
      if (index != -1) {
        LOG(FATAL) << "Error two different CT DicomSerie with the same series_uid exist. Database corrupted."
                   << std::endl
                   << "First serie: " << series_to_insert[index] << std::endl
                   << "Second serie: " << series_to_insert[i] << std::endl;
      }
      index = i;
    }
  }
  // We find a corresponding serie
  if (index != -1) {
    *serie = series_to_insert[index];
    return true;
  }

  // Find all existing DicomSerie with the same uid, in the db
  std::vector<DicomSerie> series;
  db_->Query<DicomSerie>(odb::query<DicomSerie>::dicom_series_uid == series_uid, series);
  index=-1;
  for(auto i=0; i<series.size(); i++) {
    DicomSerie * s = &series[i];
    if (modality != s->dicom_modality) continue;
    // Here we found a DicomSerie with the same series_uid
    // Very simple heuristic based on the modality
    if (s->dicom_modality == "CT") {
      if (index != -1) {
        LOG(FATAL) << "Error two different CT DicomSerie with the same series_uid exist. Database corrupted."
                   << std::endl
                   << "First serie: " << series[index] << std::endl
                   << "Second serie: " << s << std::endl;
      }
      index = i;
    }
  }

  // We find a corresponding serie
  if (index != -1) {
    *serie = &series[index];
    return true;
  }

  // Nothing found
  return false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// Do not check if the serie already exist
syd::DicomSerie * syd::DicomSerieBuilder::CreateDicomSerie(const std::string & filename, DcmObject * dset)
{
  syd::DicomSerie * serie = new syd::DicomSerie;
  UpdateDicomSerie(serie, filename, dset);
  return serie;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// Do not check if the serie already exist
void syd::DicomSerieBuilder::UpdateDicomSerie(DicomSerie * serie,
                                              const std::string & filename,
                                              DcmObject * dset)
{
  // get date
  std::string AcquisitionTime = GetTagValueString(dset, "AcquisitionTime");
  std::string AcquisitionDate = GetTagValueString(dset, "AcquisitionDate");
  std::string ContentDate = GetTagValueString(dset, "ContentDate");
  std::string ContentTime = GetTagValueString(dset, "ContentTime");
  std::string InstanceCreationDate = GetTagValueString(dset, "InstanceCreationDate");
  std::string InstanceCreationTime = GetTagValueString(dset, "InstanceCreationTime");
  std::string acquisition_date = ConvertDicomDateToStringDate(AcquisitionDate, AcquisitionTime);
  std::string reconstruction_date = ConvertDicomDateToStringDate(ContentDate, ContentTime);
  if (reconstruction_date.empty())
    reconstruction_date = ConvertDicomDateToStringDate(InstanceCreationDate, InstanceCreationTime);

  // Injection must be before the acquisition
  if (syd::DateDifferenceInHours(acquisition_date, injection_.date) < 0) {
    LOG(FATAL) << "Error, try to create a dicomserie with a date before the injection."
               << std::endl << "Injection : " << injection_
               << std::endl << "Date : " << acquisition_date;
  }

  // Patient, injection (do not check here that injection is really associated with the patient)
  std::string patientID = GetTagValueString(dset, "PatientID");
  std::string patientName = GetTagValueString(dset, "PatientName");
  LOG(3) << "Check patient dicom_patientid is the same than the given patient";
  bool b = patient_.CheckIdentity(patientID, patientName);
  if (!b and !forcePatientFlag_ and patient_.dicom_patientid != "unknown_dicom_id") {
    LOG(FATAL) << "Patient do not seems to be the same. You ask for " << patient_.name
               << " with dicom_id = '" << patient_.dicom_patientid << "'"
               << " while in dicom, it is '" << patientID << "'" << std::endl
               << "Filename is " << filename << std::endl
               << "Use 'forcePatient' if you want to bypass this check";
  }
  if (!b) {
    // if the patient has no dicom id, we set it (and update the db)
    LOG(1) << "The dicom_id of the patient " << patient_ << " has been updated to "
           << patientID << " (dicom name is " << patientName << ")";
    patient_.dicom_patientid = patientID;
    bool a = patient_.CheckIdentity(patientID, patientName);
    if (!a) {
      LOG(FATAL) << "Patient name dont match ? patient is " << patient_ << std::endl
                 << " while dicom is " << patientName;
    }
    db_->Update<Patient>(patient_);
  }
  serie->patient = std::make_shared<syd::Patient>(patient_);
  serie->injection = std::make_shared<syd::Injection>(injection_);

  // Modality
  serie->dicom_modality = GetTagValueString(dset, "Modality");

  // UID
  serie->dicom_study_uid = GetTagValueString(dset, "StudyInstanceUID");
  serie->dicom_series_uid = GetTagValueString(dset, "SeriesInstanceUID");
  serie->dicom_frame_of_reference_uid = GetTagValueString(dset, "FrameOfReferenceUID");
  //  serie->dicom_dataset_uid = GetTagValueString(dset, "DatasetUID");

  // Date
  serie->acquisition_date = acquisition_date;
  serie->reconstruction_date = reconstruction_date;

  // Description. We merge the tag because it is never consistant
  std::string SeriesDescription = GetTagValueString(dset, "SeriesDescription");
  std::string StudyDescription = GetTagValueString(dset, "StudyDescription");
  std::string ImageID = GetTagValueString(dset, "ImageID");
  std::string DatasetName = GetTagValueString(dset, "DatasetName");
  std::string description = SeriesDescription+" "+StudyDescription
    +" "+ImageID+" "+DatasetName;
  serie->dicom_description = description;

  // Device
  std::string Manufacturer = GetTagValueString(dset, "Manufacturer");
  std::string ManufacturerModelName = GetTagValueString(dset, "ManufacturerModelName");
  std::string device=Manufacturer+" "+ManufacturerModelName;
  serie->dicom_manufacturer = device;

  // Image size
  int rows = GetTagValueUShort(dset, "Rows");
  int columns = GetTagValueUShort(dset, "Columns");
  serie->size[0] = columns;
  serie->size[1] = rows;
  serie->size[2] = 0;

  int slice = atoi(GetTagValueString(dset, "NumberOfFrames").c_str());
  serie->size[2] = slice;

  // Image spacing
  double sz = GetTagValueDouble(dset, "SpacingBetweenSlices");
  if (sz == 0) sz = 1; // only one slice
  std::string spacing = GetTagValueString(dset, "PixelSpacing");
  int n = spacing.find("\\");
  std::string sx = spacing.substr(0,n);
  spacing = spacing.substr(n+1,spacing.size());
  n = spacing.find("\\");
  std::string sy = spacing.substr(0,n);
  serie->spacing[0] = atof(sx.c_str());
  serie->spacing[1] = atof(sy.c_str());
  serie->spacing[2] = sz;

  // other (needed ?)
  // std::string TableTraverse = GetTagValueString(dset, "TableTraverse");
  // std::string InstanceNumber = GetTagValueString(dset, "InstanceNumber");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// Do not check if the DicomFile and the file already exist
syd::DicomFile * syd::DicomSerieBuilder::CreateDicomFile(const std::string & filename,
                                                         DcmObject * dset,
                                                         DicomSerie * serie)
{
  // First create the file
  syd::File * file = new syd::File;
  std::string f = GetFilenameFromPath(filename);
  file->filename = f;
  std::string p = db_->GetAbsoluteFolder(*serie);
  std::string rp = p;
  syd::Replace(rp, db_->GetAbsoluteDBFolder()+PATH_SEPARATOR, ""); // Get relative folder
  file->path = rp;
  files_to_copy.push_back(filename);
  destination_folders.push_back(p);
  // file->md5 FIXME later

  // Then create the dicomfile
  syd::DicomFile * dicomfile = new syd::DicomFile;
  dicomfile->file = std::shared_ptr<syd::File>(file);
  dicomfile->dicom_serie = std::shared_ptr<syd::DicomSerie>(serie);
  std::string sop_uid = GetTagValueString(dset, "SOPInstanceUID");
  dicomfile->dicom_sop_uid = sop_uid;
  int instance_number = atoi(GetTagValueString(dset, "InstanceNumber").c_str());
  dicomfile->dicom_instance_number = instance_number;

  // Update the nb of slices
  serie->size[2]++;

  return dicomfile;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomSerieBuilder::InsertDicomSeries()
{
  // Gather the syd::Files to update
  std::vector<syd::File*> files;
  for(auto d:dicomfiles_to_insert) {
    files.push_back(d->file.get());
  }
  db_->Insert(files);

  // Copy files
  int nb_of_skip_copy=0;
  int n = files_to_copy.size();
  for(auto i=0; i<files_to_copy.size(); i++) {
    std::string f = syd::GetFilenameFromPath(files_to_copy[i]);
    std::string destination = destination_folders[i]+PATH_SEPARATOR+f;
    if (syd::FileExists(destination)) {
      LOG(3) << "Destination file already exist, ignoring";
      nb_of_skip_copy++;
      continue;
    }
    LOG(2) << "Copying " << f << " to " << destination_folders[i] << std::endl;
    syd::CopyFile(files_to_copy[i].c_str(), destination);
    syd::loadbar(i,n);
  }

  // Update the database
  db_->Insert(series_to_insert);
  db_->Insert(dicomfiles_to_insert);

  // Log
  LOG(1) << files.size() << " Files have been added in the db";
  LOG(1) << dicomfiles_to_insert.size() << " DicomFiles have been added in the db";
  LOG(1) << series_to_insert.size() << " DicomSeries has been added in the db";
  if (nb_of_skip_files != 0) {
    LOG(1) << nb_of_skip_files << " dicom already exist in the db and have been skipped.";
  }
  if (nb_of_skip_copy != 0) {
    LOG(1) << nb_of_skip_copy << " files already exist in the db folder and have not been copied.";
  }
  LOG(1) << files_to_copy.size()-nb_of_skip_copy << " files have been copied.";

  // Once done, clear vectors
  series_to_insert.clear();
  dicomfiles_to_insert.clear();
  files.clear();
  files_to_copy.clear();
  destination_folders.clear();
  nb_of_skip_files = 0;
}
// --------------------------------------------------------------------
