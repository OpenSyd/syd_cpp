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



// --------------------------------------------------------------------
syd::DicomSerieBuilder::DicomSerieBuilder(StandardDatabase * db)
{
  patient_ = NULL;
  db_ = db;
  forcePatientFlag_ = false;
  nb_of_skip_files = 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomSerieBuilder::~DicomSerieBuilder()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomSerieBuilder::SetPatient(Patient::pointer p)
{
  patient_ = p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomSerieBuilder::SearchDicomInFile(std::string filename)
{
  if (patient_ == NULL) {
    EXCEPTION("Patient must be set before using SearchDicomInFile");
  }

  itk::GDCMImageIO::Pointer dicomIO;
  try {
    dicomIO = syd::ReadDicomHeader(filename);
  } catch (...) {
    LOG(3) << sydlog::warningColor << "Warning cannot read '"
           << filename << "' (it is not a dicom file ?).";
    return;
  }

  // Test if this dicom file already exist in the db
  std::string sop_uid;
  bool b = dicomIO->GetValueFromTag("0008|0018", sop_uid); // SOPInstanceUID
  if (!b) {
    LOG(WARNING) << "Cannot find tag 0008|0018 SOPInstanceUID. Ignored";
    return;
  }
  if (DicomFileAlreadyExist(sop_uid)) {
    LOG(2) << "Dicom file with same sop_uid already exist in the db. Skipping " << filename;
    nb_of_skip_files++;
    return;
  }

  // Test if a serie already exist in the database
  DicomSerie::pointer serie;
  b = GuessDicomSerieForThisFile(filename, dicomIO, serie);

  // If this is a new DicomSerie, we create it
  if (!b) {
    db_->New(serie);
    UpdateDicomSerie(serie, filename, dicomIO);
    series_to_insert.push_back(serie);
    LOG(2) << "Creating a new serie: " << serie->dicom_series_uid;
  }

  // Then we add this dicomfile to the serie
  DicomFile::pointer dicomfile = CreateDicomFile(filename, dicomIO, serie);
  dicomfiles_to_insert.push_back(dicomfile);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::DicomSerieBuilder::DicomFileAlreadyExist(const std::string & sop_uid)
{
  DicomFile::vector df;
  odb::query<DicomFile> q = odb::query<DicomFile>::dicom_sop_uid == sop_uid;
  db_->Query(df, q);
  int n = df.size();
  if (n>0) return true;
  for(auto f:dicomfiles_to_insert) {
    if (f->dicom_sop_uid == sop_uid) return true;
  }
  return false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::DicomSerieBuilder::GuessDicomSerieForThisFile(const std::string & filename,
                                                        itk::GDCMImageIO::Pointer dicomIO,
                                                        DicomSerie::pointer & serie)
{
  // Check in the future series
  std::string series_uid;
  bool b = dicomIO->GetValueFromTag("0020|000e", series_uid); // SeriesInstanceUID
  if (!b) {
    LOG(WARNING) << "Cannot find tag 00020|000a SeriesInstanceUID. Ignored";
    return false;
  }
  std::string modality;
  b = dicomIO->GetValueFromTag("0008|0060", modality); // Modality
  if (!b) {
    LOG(WARNING) << "Cannot find tag 0008|0060 Modality. Ignored";
    return false;
  }

  int index = -1;
  for(auto i=0; i<series_to_insert.size(); i++) {
    DicomSerie::pointer s = series_to_insert[i];
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
    serie = series_to_insert[index];
    return true;
  }

  // Find all existing DicomSerie with the same uid, in the db
  DicomSerie::vector series;
  odb::query<DicomSerie> q = odb::query<DicomSerie>::dicom_series_uid == series_uid;
  db_->Query(series, q);
  index=-1;
  for(auto i=0; i<series.size(); i++) {
    auto s = series[i];
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
    serie = series[index];
    return true;
  }

  // Nothing found
  return false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// Update the field
void syd::DicomSerieBuilder::UpdateDicomSerie(DicomSerie::pointer serie)
{
  if (serie->dicom_files.size() == 0) {
    EXCEPTION("No DicomFile for this DicomSerie");
  }

  // Set the current patient
  patient_ = serie->patient;

  // Open the first file
  auto filename = serie->dicom_files[0]->GetAbsolutePath();
  itk::GDCMImageIO::Pointer dicomIO;
  try {
    dicomIO = syd::ReadDicomHeader(filename);
  } catch (std::exception & e) {
    LOG(WARNING) << "Error cannot read '" << filename << "' (it is not a dicom file ?).";
    return;
  }
  UpdateDicomSerie(serie, filename, dicomIO);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// Do not check if the serie already exist
void syd::DicomSerieBuilder::UpdateDicomSerie(DicomSerie::pointer serie,
                                              const std::string & filename,
                                              itk::GDCMImageIO::Pointer dicomIO)
{
  // get date
  std::string AcquisitionTime =
    GetTagValueFromTagKey(dicomIO, "0008|0032", empty_value); // Acquisition Time
  std::string AcquisitionDate =
    GetTagValueFromTagKey(dicomIO, "0008|0022", empty_value); // Acquisition Date
  std::string ContentDate =
    GetTagValueFromTagKey(dicomIO, "0008|0023", empty_value); // Content Date
  std::string ContentTime =
    GetTagValueFromTagKey(dicomIO, "0008|0033", empty_value); // Content Time
  std::string InstanceCreationDate =
    GetTagValueFromTagKey(dicomIO, "0008|0012", empty_value); // Instance Creation Date
  std::string InstanceCreationTime =
    GetTagValueFromTagKey(dicomIO, "0008|0013", empty_value); //Instance Creation Time
  std::string acquisition_date =
    ConvertDicomDateToStringDate(AcquisitionDate, AcquisitionTime);
  std::string reconstruction_date =
    ConvertDicomDateToStringDate(ContentDate, ContentTime);

  if (reconstruction_date.empty())
    reconstruction_date = ConvertDicomDateToStringDate(InstanceCreationDate, InstanceCreationTime);
  if (acquisition_date.empty())
    acquisition_date = ConvertDicomDateToStringDate(InstanceCreationDate, InstanceCreationTime);

  // Patient
  std::string patientID =
    GetTagValueFromTagKey(dicomIO, "0010|0020", empty_value); // Patient ID
  std::string patientName =
    GetTagValueFromTagKey(dicomIO, "0010|1001", empty_value); //Patient Name
  char * sex = new char[100];
  dicomIO->GetPatientSex(sex);

  if (!forcePatientFlag_) {
    LOG(3) << "Check patient dicom_patientid is the same than the given patient";
    if (patient_->dicom_patientid != patientID) {
      LOG(FATAL) << "Patient does not seems to be the same. You ask for " << patient_->name
                 << " with dicom_id = " << patient_->dicom_patientid
                 << " while in dicom, it is '" << patientID
                 << " with name: " << patientName << std::endl
                 << "Filename is " << filename << std::endl
                 << "Use 'forcePatient' if you want to bypass this check";
    }
    if (patient_->sex != std::string(sex)) {
      LOG(FATAL) << "Patient's sex is different in the db and in the dicom: "
                 << patient_->sex << " vs " << sex;
    }
  }
  if (patient_->dicom_patientid != patientID) {
    // update the dicom id if it is different (force flag)
    patient_->dicom_patientid = patientID;
    patient_->sex = sex;
    db_->Update<Patient>(patient_);
  }
  serie->patient = patient_;

  // Modality
  serie->dicom_modality =
    GetTagValueFromTagKey(dicomIO, "0008|0060", empty_value); //Modality

  // UID
  serie->dicom_study_uid =
    GetTagValueFromTagKey(dicomIO, "0020|000d", empty_value); //StudyInstanceUID
  serie->dicom_series_uid =
    GetTagValueFromTagKey(dicomIO, "0020|000e", empty_value); //SeriesInstanceUID
  serie->dicom_frame_of_reference_uid =
    GetTagValueFromTagKey(dicomIO, "0020|0052", empty_value);//FrameOfReferenceUID
  //  what about "DatasetUID ?

  // Date
  serie->dicom_acquisition_date = acquisition_date;
  serie->dicom_reconstruction_date = reconstruction_date;

  // Description. We merge the tag because it is never consistant
  std::string SeriesDescription =
    GetTagValueFromTagKey(dicomIO, "0008|103e", empty_value); // SeriesDescription
  std::string StudyDescription =
    GetTagValueFromTagKey(dicomIO, "0008|1030", empty_value); // StudyDescription
  std::string ImageID =
    GetTagValueFromTagKey(dicomIO, "0054|0400", empty_value); // Image ID
  std::string DatasetName =
    GetTagValueFromTagKey(dicomIO, "0011|1012", empty_value); // DatasetName
  std::string description =
    SeriesDescription+" "+StudyDescription
    +" "+ImageID+" "+DatasetName;

  // Device
  std::string Manufacturer =
    GetTagValueFromTagKey(dicomIO, "0008|0070", empty_value); // Manufacturer
  std::string ManufacturerModelName =
    GetTagValueFromTagKey(dicomIO, "0008|1090", empty_value); //ManufacturerModelName
  description = description + " " + Manufacturer + " " + ManufacturerModelName;

  // Store description
  serie->dicom_description = description;

  // Image spacing
  /*
    double sz = GetTagValueDouble(dset, "SpacingBetweenSlices");
    if (sz == 0) {
    sz = GetTagValueDouble(dset, "SliceThickness");
    if (sz == 0) sz = 1.0; // not found, default
    }
    std::string spacing = GetTagValueFromTagKey(dicomIO, "PixelSpacing");
    int n = spacing.find("\\");
    std::string sx = spacing.substr(0,n);
    spacing = spacing.substr(n+1,spacing.size());
    n = spacing.find("\\");
    std::string sy = spacing.substr(0,n);
    // serie->spacing[0] = atof(sx.c_str());
    // serie->spacing[1] = atof(sy.c_str());
    // serie->spacing[2] = sz;
    // if (serie->spacing[0] == 0) serie->spacing[0] = 1.0;
    // if (serie->spacing[1] == 0) serie->spacing[1] = 1.0;
    //  if (sz != 0) serie->spacing[2] = sz; // only update if found
    */
  //    DD("TODO spacing");

  // other (needed ?)
  // std::string TableTraverse = GetTagValueFromTagKey(dicomIO, "TableTraverse");
  // std::string InstanceNumber = GetTagValueFromTagKey(dicomIO, "InstanceNumber");

  // Pixel scale
  // double ps = 1.0;
  // ps = GetTagValueFromTagKey(dicomIO, "0011|103b", 1.0); // PixelScale
  // if (ps == 0.0) ps = 1.0;
  // serie->dicom_pixel_scale = ps;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// Do not check if the DicomFile and the file already exist
syd::DicomFile::pointer syd::DicomSerieBuilder::
CreateDicomFile(const std::string & filename,
                itk::GDCMImageIO::Pointer dicomIO,
                DicomSerie::pointer serie)
{
  // First create the file
  syd::DicomFile::pointer dicomfile;
  db_->New(dicomfile);
  std::string f = GetFilenameFromPath(filename);
  dicomfile->filename = f;

  std::string relative_folder = serie->ComputeRelativeFolder(); // FIXME
  std::string absolute_folder = db_->ConvertToAbsolutePath(relative_folder);
  if (!fs::exists(absolute_folder)) fs::create_directories(absolute_folder);

  dicomfile->path = relative_folder;
  files_to_copy.push_back(filename);
  destination_folders.push_back(absolute_folder);

  // Then create the dicomfile
  //    dicomfile->dicom_serie = serie;
  std::string sop_uid =
    GetTagValueFromTagKey(dicomIO, "0008|0018", empty_value); //SOPInstanceUID
  dicomfile->dicom_sop_uid = sop_uid;

  std::string in = GetTagValueFromTagKey(dicomIO, "0020|0013", empty_value);//InstanceNumber
  int instance_number;
  if (in == empty_value) instance_number = 1;
  else instance_number = atoi(in.c_str());
  dicomfile->dicom_instance_number = instance_number;

  // Update the nb of slices
  // Image size
  // unsigned int rows = GetTagValueFromTagKey(dicomIO, "0028|0010", 0); // Rows
  // unsigned int columns = GetTagValueFromTagKey(dicomIO, "0028|0011", 0); // Columns
  // description = ToString(rows) + "x" +
  // int slice = atoi(GetTagValueFromTagKey(dicomIO, "NumberOfFrames").c_str());
  // if (slice != 0) serie->size[2] = slice;
  // else serie->size[2]++;
  //    DD("update size");
  serie->dicom_files.push_back(dicomfile);
  return dicomfile;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomSerieBuilder::InsertDicomSeries()
{
  // Update the database first to get the File id
  db_->Insert(dicomfiles_to_insert); // must be before serie
  db_->Insert(series_to_insert);
  assert(dicomfiles_to_insert.size() == files_to_copy.size());

  // Copy files
  int nb_of_skip_copy=0;
  int n = files_to_copy.size();
  for(auto i=0; i<files_to_copy.size(); i++) {
    std::string f = GetFilenameFromPath(files_to_copy[i]);

    // Add the id at the beginning of the file to insure unicity
    std::stringstream dss;
    dss << destination_folders[i] << PATH_SEPARATOR
        << "dcm_" << dicomfiles_to_insert[i]->id << "_" << f;
    dicomfiles_to_insert[i]->filename =
      "dcm_" + std::to_string(dicomfiles_to_insert[i]->id) +
      "_" +dicomfiles_to_insert[i]->filename;
    std::string destination =dss.str();
    if (fs::exists(destination)) {
      LOG(3) << "Destination file already exist, ignoring";
      nb_of_skip_copy++;
      continue;
    }
    LOG(3) << "Copying " << f << " to " << destination_folders[i] << std::endl;
    fs::copy_file(files_to_copy[i].c_str(), destination);
    loadbar(i,n);
  }

  // Update because the filename changed
  db_->Update(dicomfiles_to_insert);

  // Log
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
  //files.clear();
  files_to_copy.clear();
  destination_folders.clear();
  nb_of_skip_files = 0;
}
// --------------------------------------------------------------------
