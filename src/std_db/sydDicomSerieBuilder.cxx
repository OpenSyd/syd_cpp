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
#include "sydDicomSerieHelper.h"

// --------------------------------------------------------------------
syd::DicomSerieBuilder::DicomSerieBuilder(syd::StandardDatabase * db_)
{
  nb_of_skip_files = 0;
  db = db_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomSerieBuilder::~DicomSerieBuilder()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomSerieBuilder::SearchDicomInFile(std::string filename,
                                               syd::Patient::pointer patient,
                                               bool update_patient_info_from_file_flag)
{
  itk::GDCMImageIO::Pointer dicomIO;
  try {
    dicomIO = syd::ReadDicomHeader(filename);
  } catch (std::exception & e) {
    LOG(3) << sydlog::warningColor << "Warning cannot read '"
           << filename << "' (it is not a dicom image file ?). " << e.what();
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
    serie = db->New<syd::DicomSerie>();
    UpdateDicomSerie(serie, filename, dicomIO);
    // Set patient
    if (patient != nullptr) CheckAndSetPatient(serie, patient);
    else {
      syd::GuessAndSetPatient(serie);
      patient = serie->patient;
    }
    // Update patient
    if (update_patient_info_from_file_flag) {
      syd::SetPatientInfoFromDicom(serie, serie->patient);
      db->Update(serie->patient);
    }
    // Create folder if needed
    std::string relative_folder = serie->ComputeRelativeFolder();
    std::string absolute_folder = db->ConvertToAbsolutePath(relative_folder);
    if (!fs::exists(absolute_folder)) fs::create_directories(absolute_folder);

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
  db->Query(df, q);
  int n = df.size();
  if (n>0) return true;
  for(auto f:dicomfiles_to_insert)
    if (f->dicom_sop_uid == sop_uid) return true;
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
  db->Query(series, q);
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
    GetTagValueFromTagKey(dicomIO, "0010|0010", empty_value); // Patient Name
  char * sex = new char[100];
  dicomIO->GetPatientSex(sex);

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

  // Patient info
  serie->dicom_patient_name = trim(patientName);
  serie->dicom_patient_id = trim(patientID);
  serie->dicom_patient_sex = sex;
  serie->dicom_patient_birth_date =
    GetTagValueFromTagKey(dicomIO, "0010|0030", empty_value); // Patient's Birth Date

  // Description. We merge the tag because it is never consistant
  std::string SeriesDescription =
    GetTagValueFromTagKey(dicomIO, "0008|103e", empty_value); // SeriesDescription
  std::string StudyDescription =
    GetTagValueFromTagKey(dicomIO, "0008|1030", empty_value); // StudyDescription
  std::string ImageID =
    GetTagValueFromTagKey(dicomIO, "0054|0400", empty_value); // Image ID
  std::string DatasetName =
    GetTagValueFromTagKey(dicomIO, "0011|1012", empty_value); // DatasetName
  std::string StudyID =
    GetTagValueFromTagKey(dicomIO, "0020|0010", empty_value); // StudyID
  std::string StudyName =
    GetTagValueFromTagKey(dicomIO, "0009|1010", empty_value); // StudyName
  std::string description =
    SeriesDescription+" "+StudyDescription
    +" "+ImageID+" "+DatasetName+" "+StudyID+" "+StudyName;

  // Device
  std::string Manufacturer =
    GetTagValueFromTagKey(dicomIO, "0008|0070", empty_value); // Manufacturer
  std::string ManufacturerModelName =
    GetTagValueFromTagKey(dicomIO, "0008|1090", empty_value); //ManufacturerModelName
  description = description + " " + Manufacturer + " " + ManufacturerModelName;
  description = trim(description);

  // Store description
  serie->dicom_description = description;

  serie->dicom_series_description = SeriesDescription;
  serie->dicom_study_description = StudyDescription;
  serie->dicom_image_id = ImageID;
  serie->dicom_dataset_name = DatasetName;
  serie->dicom_manufacturer = Manufacturer;
  serie->dicom_manufacturer_model_name = ManufacturerModelName;
  serie->dicom_study_id = StudyID;
  serie->dicom_software_version =
    GetTagValueFromTagKey(dicomIO, "0018|1020", empty_value); // Software version

  // Image spacing
  double sz = GetTagDoubleValueFromTagKey(dicomIO, "0018|0088", 0.0); // SpacingBetweenSlices
  if (sz == 0) {
    sz = GetTagDoubleValueFromTagKey(dicomIO, "0018|0050", 0.0); // SliceThickness
    if (sz == 0) sz = 1.0; // not found, default
  }
  std::string spacing = GetTagValueFromTagKey(dicomIO, "0028|0030", empty_value); // PixelSpacing
  int n = spacing.find("\\");
  std::string sx = spacing.substr(0,n);
  spacing = spacing.substr(n+1,spacing.size());
  n = spacing.find("\\");
  std::string sy = spacing.substr(0,n);
  serie->dicom_spacing.resize(3);
  serie->dicom_spacing[0] = atof(sx.c_str());
  serie->dicom_spacing[1] = atof(sy.c_str());
  serie->dicom_spacing[2] = sz;
  if (serie->dicom_spacing[0] == 0) serie->dicom_spacing[0] = 1.0;
  if (serie->dicom_spacing[1] == 0) serie->dicom_spacing[1] = 1.0;
  if (sz != 0) serie->dicom_spacing[2] = sz; // only update if found

  // Image size
  serie->dicom_size.resize(3); // will be updated in CreateDicomFile
  unsigned int rows = GetTagDoubleValueFromTagKey(dicomIO, "0028|0010", 0); // Rows
  unsigned int columns = GetTagDoubleValueFromTagKey(dicomIO, "0028|0011", 0); // Columns
  int slice = GetTagDoubleValueFromTagKey(dicomIO, "0028|0008", 0); // Number of Frames
  serie->dicom_size[0] = rows;
  serie->dicom_size[1] = columns;
  serie->dicom_size[2] = slice; // will be updated in CreateDicomFile if == 0

  // Pixel scale
  double ps = 1.0;
  ps = GetTagDoubleValueFromTagKey(dicomIO, "0011|103b", 1.0); // PixelScale
  if (ps == 0.0) ps = 1.0;
  serie->dicom_pixel_scale = ps;
  double po = 0.0;
  po = GetTagDoubleValueFromTagKey(dicomIO, "0011|103c", 0.0); // PixelOffset
  serie->dicom_pixel_offset = po;

  // Window/level
  double wc = GetTagDoubleValueFromTagKey(dicomIO, "0028|1050", 0.0); // WindowCenter
  double ww = GetTagDoubleValueFromTagKey(dicomIO, "0028|1051", 0.0); // WindowWidth
  serie->dicom_window_center = wc;
  serie->dicom_window_width = ww;

  // Specific tag for NM
  serie->dicom_table_traverse_in_mm =
    GetTagDoubleValueFromTagKey(dicomIO, "0018|1131", 0.0); // TableTraverse
  serie->dicom_table_height_in_mm =
    GetTagDoubleValueFromTagKey(dicomIO, "0018|1130", 0.0); // TableHeight
  serie->dicom_radionuclide_name =
    GetTagValueFromTagKey(dicomIO, "0011|100d", empty_value); // RadionuclideName
  serie->dicom_counts_accumulated =
    GetTagDoubleValueFromTagKey(dicomIO, "0018|0070", 0); // CountsAccumulated
  serie->dicom_actual_frame_duration_in_msec =
    GetTagDoubleValueFromTagKey(dicomIO, "0018|1242", 0.0); // ActualFrameDuration
  serie->dicom_number_of_frames_in_rotation =
    GetTagDoubleValueFromTagKey(dicomIO, "0054|0053", 0); // NumberOfFramesInRotation
  serie->dicom_number_of_rotations =
    GetTagDoubleValueFromTagKey(dicomIO, "0054|0051", 0); // NumberOfRotations
  serie->dicom_rotation_angle =
    GetTagDoubleValueFromTagKey(dicomIO, "0070|0230", 0.0); // RotationAngle
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
  auto dicomfile = db->New<syd::DicomFile>();
  std::string f = GetFilenameFromPath(filename);
  dicomfile->filename = f;
  std::string relative_folder = serie->ComputeRelativeFolder();
  dicomfile->path = relative_folder;
  files_to_copy.push_back(filename);

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
  int slice = GetTagDoubleValueFromTagKey(dicomIO, "0028|0008", 0); // Number of Frames
  // if no NumberOfFrames, we count the nb of dicomfile
  if (slice == 0) serie->dicom_size[2] = serie->dicom_files.size()+1;

  // Update dicom file
  serie->dicom_files.push_back(dicomfile);
  return dicomfile;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomSerie::vector syd::DicomSerieBuilder::InsertDicomSeries()
{
  // Update the database first to get the File id
  db->Insert(dicomfiles_to_insert); // must be before serie
  db->Insert(series_to_insert);
  assert(dicomfiles_to_insert.size() == files_to_copy.size());

  // Copy files
  int nb_of_skip_copy=0;
  int n = files_to_copy.size();
  for(auto i=0; i<files_to_copy.size(); i++) {
    std::string f = GetFilenameFromPath(files_to_copy[i]);

    // Add the id at the beginning of the file to insure unicity
    std::stringstream dss;
    auto destination_folder = db->ConvertToAbsolutePath(dicomfiles_to_insert[i]->path);
    dss << destination_folder << PATH_SEPARATOR
        << "dcm_" << dicomfiles_to_insert[i]->id << "_" << f;
    dicomfiles_to_insert[i]->filename =
      "dcm_" + std::to_string(dicomfiles_to_insert[i]->id) +
      "_" +dicomfiles_to_insert[i]->filename;
    std::string destination = dss.str();
    if (fs::exists(destination)) {
      LOG(3) << "Destination file already exist, ignoring";
      nb_of_skip_copy++;
      continue;
    }
    LOG(3) << "Copying " << f << " to " << destination_folder << std::endl;
    fs::copy_file(files_to_copy[i].c_str(), destination);
    loadbar(i,n);
  }

  // Update because the filename changed
  db->Update(dicomfiles_to_insert);

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
  dicomfiles_to_insert.clear();
  files_to_copy.clear();
  nb_of_skip_files = 0;

  return series_to_insert;
}
// --------------------------------------------------------------------
