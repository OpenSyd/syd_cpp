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
#include "sydDicomBuilder.h"
#include "sydDicomSerieHelper.h"

//gdcm
#include "gdcmReader.h"
#include "gdcmDataElement.h"
#include "gdcmTag.h"

// --------------------------------------------------------------------
syd::DicomBuilder::DicomBuilder(syd::StandardDatabase * db_,
                                bool update_patient_info_from_file_flag_)
{
  db = db_;
  nb_of_skip_files = 0;
  nb_of_skip_copy = 0;
  update_patient_info_from_file_flag = update_patient_info_from_file_flag_;
  force_overwrite_existing_dicomfile_flag = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomBuilder::~DicomBuilder()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomBuilder::SearchDicomInFile(std::string filename,
                                          syd::Patient::pointer patient)
{
  try {
    SearchDicomSerieInFile(filename, patient);
  } catch (std::exception & e) {
    try {
      SearchDicomStructInFile(filename, patient);
    } catch (std::exception & e) {
      LOG(3) << sydlog::warningColor
             << "Warning cannot read/insert " << filename
             << std::endl << e.what();
      return;
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomBuilder::SearchDicomSerieInFile(std::string filename,
                                               syd::Patient::pointer patient)
{
  auto dicomIO = syd::ReadDicomHeader(filename); // will raise exception if not ok

  // Test if this dicom file already exist in the db
  std::string sop_uid;
  bool b = dicomIO->GetValueFromTag("0008|0018", sop_uid); // SOPInstanceUID
  if (!b) {
    LOG(WARNING) << "Cannot find tag 0008|0018 SOPInstanceUID. Skipping " << filename;
    nb_of_skip_files++;
    return;
  }
  StripNullTerminatedChar(sop_uid);
  if (FindDicomFile(sop_uid) != nullptr) {
    if (!force_overwrite_existing_dicomfile_flag) {
      LOG(WARNING) << "Dicom file with same sop_uid already exist in the db. Skipping " << filename;
      nb_of_skip_files++;
      return;
    }
    auto df = FindDicomFile(sop_uid);
    try {
      db->Delete(df);
    } catch(std::exception & e) {
      LOG(WARNING) << "Dicom file with same sop_uid already exist and cannot be removed. Skipping " << filename;
      nb_of_skip_files++;
      return;
    }
  }

  // Test if a serie already exist in the database
  DicomSerie::pointer serie;
  b = GuessDicomSerieForThisFile(filename, dicomIO, serie);
  // If this is a new DicomSerie, we create it
  if (!b) {
    serie = db->New<syd::DicomSerie>();
    UpdateDicomSerie(serie, filename, dicomIO);
    SetDicomPatient(serie, patient);
    if (update_patient_info_from_file_flag) db->Update(serie->patient);
    dicom_series_to_insert.push_back(serie);
    LOG(2) << "Creating DicomSerie " << serie->dicom_series_uid;
  }

  // Then we add this dicomfile to the serie
  DicomFile::pointer dicomfile = CreateDicomFile(filename, dicomIO, serie);

  // Ready to insert
  dicom_series_filenames_to_copy.push_back(filename);
  dicom_series_dicom_files_to_insert.push_back(dicomfile);
  dicom_files_corresponding_series.push_back(serie);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomFile::pointer syd::DicomBuilder::FindDicomFile(const std::string & sop_uid)
{
  DicomFile::vector df;
  odb::query<DicomFile> q = odb::query<DicomFile>::dicom_sop_uid == sop_uid;
  db->Query(df, q);
  int n = df.size();
  if (n>0) return df[0];
  for(auto f:dicom_series_dicom_files_to_insert)
    if (f->dicom_sop_uid == sop_uid) return f;
  return nullptr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::DicomBuilder::GuessDicomSerieForThisFile(const std::string & filename,
                                                   itk::GDCMImageIO::Pointer dicomIO,
                                                   DicomSerie::pointer & serie)
{
  // Check in the future series
  std::string series_uid;
  bool b = dicomIO->GetValueFromTag("0020|000e", series_uid); // SeriesInstanceUID
  if (!b) {
    LOG(WARNING) << "Cannot find tag 0020|000e SeriesInstanceUID. Skipping " << filename;
    nb_of_skip_files++;
    return false;
  }
  StripNullTerminatedChar(series_uid);
  std::string modality;
  b = dicomIO->GetValueFromTag("0008|0060", modality); // Modality
  if (!b) {
    LOG(WARNING) << "Cannot find tag 0008|0060 Modality. Skipping " << filename;
    nb_of_skip_files++;
    return false;
  }
  StripNullTerminatedChar(modality);
  std::string nbSlice;
  b = dicomIO->GetValueFromTag("0028|0008", nbSlice); // Modality
  if (!b)
    nbSlice = "1"; //For CT slices the tag doesn't exist
  StripNullTerminatedChar(nbSlice);
  int index = -1;
  for(auto i=0; i<dicom_series_to_insert.size(); i++) {
    DicomSerie::pointer s = dicom_series_to_insert[i];
    // First check series_uid and modality
    if (s->dicom_series_uid != series_uid) continue;
    if (modality != s->dicom_modality) continue;
    if (nbSlice != "1") continue;

    // Here we found a DicomSerie with the same series_uid
    // Very simple heuristic based on the modality
    // And the number of slices of dicomIO has to be 1 (to avoid to merge 2 different and complete images)
    if (s->dicom_modality == "CT" or s->dicom_modality == "PT") {
      if (index != -1) {
        LOG(FATAL) << "Error two different DicomSerie with the same series_uid exist. Database corrupted."
                   << std::endl
                   << "First serie: " << dicom_series_to_insert[index] << std::endl
                   << "Second serie: " << dicom_series_to_insert[i] << std::endl;
      }
      index = i;
    }
  }
  // We find a corresponding serie
  if (index != -1) {
    serie = dicom_series_to_insert[index];
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
        LOG(FATAL) << "Error two different DicomSerie with the same series_uid exist. Database corrupted."
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
void syd::DicomBuilder::UpdateDicomSerie(DicomSerie::pointer serie)
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
void syd::DicomBuilder::UpdateDicomSerie(DicomSerie::pointer serie,
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
    GetTagValueFromTagKey(dicomIO, "0008|0013", empty_value); // Instance Creation Time
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

  // Real world value intercept/slope (for NM)
  double slope = 0.0;
  double intercept = 0.0;
  slope = GetTagDoubleValueFromTagKey(dicomIO, "0040|9225", 0.0); // Real world value slope
  intercept = GetTagDoubleValueFromTagKey(dicomIO, "0040|9224", 0.0); // Real world value intercept
  if (slope == 0.0) { //look with gdcm
    auto reader = syd::GetDicomReader(filename);
    auto dataset = reader.GetFile().GetDataSet();
    try {
      gdcm::SmartPointer<gdcm::SequenceOfItems> sqi = GetSequence(dataset, 0x0040, 0x9096);
      slope = GetTagValueFromSequence<double>(sqi, 0x0040, 0x9225);
      intercept = GetTagValueFromSequence<double>(sqi, 0x0040, 0x9224);
    }
    catch(const std::exception & e) {
      slope = 1.0;
    }
  }
  else slope = 1.0;
  serie->dicom_real_world_value_slope = slope;
  serie->dicom_real_world_value_intercept = intercept;

  //Energy Window Name
  int energyWindowNumber = 0;
  energyWindowNumber = GetTagDoubleValueFromTagKey(dicomIO, "0054|0011", 0);
  std::string tempEnergyWindowName = "";
  if (energyWindowNumber > 0) {
    auto reader = syd::GetDicomReader(filename);
    auto dataset = reader.GetFile().GetDataSet();
    for (int i=0; i<energyWindowNumber; ++i) {
      try {
        gdcm::SmartPointer<gdcm::SequenceOfItems> sqi = GetSequence(dataset, 0x0054, 0x0012);
        gdcm::Item item = sqi->GetItem(i+1);
        gdcm::Tag tag(0x0054, 0x0018);
        tempEnergyWindowName = tempEnergyWindowName + item.GetDataElement(tag).GetByteValue()->GetPointer() + " ";
      }
      catch(const std::exception & e) {}
    }
    serie->dicom_energy_window_name = tempEnergyWindowName;
    description = description + serie->dicom_energy_window_name;
  }

  // Window/level
  double wc = GetTagDoubleValueFromTagKey(dicomIO, "0028|1050", 0.0); // WindowCenter
  double ww = GetTagDoubleValueFromTagKey(dicomIO, "0028|1051", 0.0); // WindowWidth
  serie->dicom_window_center = wc;
  serie->dicom_window_width = ww;

  // Specific tag for NM
  serie->dicom_radionuclide_name =
    GetTagValueFromTagKey(dicomIO, "0011|100d", empty_value); // RadionuclideName
  serie->dicom_counts_accumulated =
    GetTagDoubleValueFromTagKey(dicomIO, "0018|0070", 0); // CountsAccumulated
  double tableHeight(0.0), tableTraverse(0.0), frameDuration(0.0);
  unsigned short nbFrameInRotation(0);
  tableHeight = GetTagDoubleValueFromTagKey(dicomIO, "0018|1130", 0.0); // TableHeight
  if (tableHeight == 0.0) { //look with gdcm
    auto reader = syd::GetDicomReader(filename);
    auto dataset = reader.GetFile().GetDataSet();
    try {
      gdcm::SmartPointer<gdcm::SequenceOfItems> sqi = GetSequence(dataset, 0x0054, 0x0052);
      tableHeight = GetTagValueFromStringSequence<double>(sqi, 0x0018, 0x1130);
    }
    catch(const std::exception & e) {
      tableHeight = 0.0;
    }
  }
  tableTraverse = GetTagDoubleValueFromTagKey(dicomIO, "0018|1131", 0.0); // TableTraverse
  if (tableTraverse == 0.0) { //look with gdcm
    auto reader = syd::GetDicomReader(filename);
    auto dataset = reader.GetFile().GetDataSet();
    try {
      gdcm::SmartPointer<gdcm::SequenceOfItems> sqi = GetSequence(dataset, 0x0054, 0x0052);
      tableTraverse = GetTagValueFromStringSequence<double>(sqi, 0x0018, 0x1131);
    }
    catch(const std::exception & e) {
      tableTraverse = 0.0;
    }
  }
  frameDuration = GetTagDoubleValueFromTagKey(dicomIO, "0018|1242", 0.0); // ActualFrameDuration
  if (frameDuration == 0.0) { //look with gdcm
    auto reader = syd::GetDicomReader(filename);
    auto dataset = reader.GetFile().GetDataSet();
    try {
      gdcm::SmartPointer<gdcm::SequenceOfItems> sqi = GetSequence(dataset, 0x0054, 0x0052);
      frameDuration = GetTagValueFromStringSequence<double>(sqi, 0x0018, 0x1242);
    }
    catch(const std::exception & e) {
      frameDuration = 1;
    }
  }
  nbFrameInRotation = GetTagDoubleValueFromTagKey(dicomIO, "0054|0053", 0); // NumberOfFramesInRotation
  if (nbFrameInRotation == 0) { //look with gdcm
    auto reader = syd::GetDicomReader(filename);
    auto dataset = reader.GetFile().GetDataSet();
    try {
      gdcm::SmartPointer<gdcm::SequenceOfItems> sqi = GetSequence(dataset, 0x0054, 0x0052);
      nbFrameInRotation = GetTagValueFromSequence<unsigned short>(sqi, 0x0054, 0x0053);
    }
    catch(const std::exception & e) {
      nbFrameInRotation = 1;
    }
  }
  serie->dicom_table_height_in_mm = tableHeight;
  serie->dicom_table_traverse_in_mm = tableTraverse;
  serie->dicom_actual_frame_duration_in_msec = frameDuration;
  serie->dicom_number_of_frames_in_rotation = nbFrameInRotation;
  serie->dicom_rotation_angle =
    GetTagDoubleValueFromTagKey(dicomIO, "0070|0230", 0.0); // RotationAngle
  int NumberOfRotations = GetTagDoubleValueFromTagKey(dicomIO, "0054|0051", 0); // NumberOfRotations
  if (NumberOfRotations == 0) { //try with gdcm
    auto reader = syd::GetDicomReader(filename);
    auto dataset = reader.GetFile().GetDataSet();
    try {
      gdcm::SmartPointer<gdcm::SequenceOfItems> sqi = GetSequence(dataset, 0x0040, 0x9096);
      NumberOfRotations = GetTagValueFromSequence<int>(sqi, 0x0054, 0x0051);
    }
    catch(const std::exception & e){}
  }
  serie->dicom_number_of_rotations = NumberOfRotations;

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

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// Do not check if the DicomFile and the file already exist
syd::DicomFile::pointer syd::DicomBuilder::
CreateDicomFile(const std::string & filename,
                itk::GDCMImageIO::Pointer dicomIO,
                DicomSerie::pointer serie)
{
  // First create the file
  auto dicomfile = db->New<syd::DicomFile>();

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
void syd::DicomBuilder::SearchDicomStructInFile(std::string filename,
                                                syd::Patient::pointer patient)
{
  auto reader = syd::ReadDicomStructHeader(filename); // will raise exception if not ok
  auto dataset = reader.GetFile().GetDataSet();

  // Test if this dicom file already exist in the db
  auto dicom_struct = CreateDicomStruct(dataset, filename);
  if (dicom_struct == nullptr) return;

  // Update Patient
  UpdateDicomStructPatient(dicom_struct, dataset);
  SetDicomPatient(dicom_struct, patient);
  if (update_patient_info_from_file_flag) db->Update(dicom_struct->patient);

  // Update
  UpdateDicomStruct(dicom_struct, dataset);
  auto dicom_file = dicom_struct->dicom_files[0]; // at least one

  // Ready to be inserted
  dicom_struct_to_insert.push_back(dicom_struct);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomStruct::pointer
syd::DicomBuilder::CreateDicomStruct(const gdcm::DataSet & dataset, std::string filename)
{
  // Series Instance UID tag
  auto serie_uid = syd::GetTagValueAsString<0x20,0x0e>(dataset);
  syd::DicomStruct::vector structs;
  odb::query<DicomStruct> q = odb::query<DicomStruct>::dicom_series_uid == serie_uid;
  db->Query(structs, q);

  if (structs.size() > 0) {
    LOG(WARNING) << "DicomStruct with similar serie_uid already exist. Skipping " << filename;
    ++nb_of_skip_files;
    return nullptr;
  }

  // Get or create DicomStruct
  LOG(2) << "Creating DicomStruct " << serie_uid;
  auto dicom_struct = db->New<syd::DicomStruct>();

  // SOP Instance UID tag
  auto sop_uid = syd::GetTagValueAsString<0x08,0x18>(dataset);
  auto dicom_file = FindDicomFile(sop_uid);
  if (dicom_file == nullptr) {
    LOG(3) << "Create a new DicomFile " << sop_uid;
    dicom_file = db->New<syd::DicomFile>();
    dicom_struct_dicom_files_to_insert.push_back(dicom_file);
    dicom_struct_filenames_to_copy.push_back(filename);
  }

  // Set DicomFile
  dicom_struct->dicom_files.clear();
  dicom_struct->dicom_files.push_back(dicom_file);
  return dicom_struct;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomBuilder::UpdateDicomStructPatient(syd::DicomStruct::pointer dicom_struct,
                                                 const gdcm::DataSet & dataset)
{
  auto s = syd::GetTagValueAsString<0x10,0x10>(dataset);
  dicom_struct->dicom_patient_name = syd::GetTagValueAsString<0x10,0x10>(dataset);
  dicom_struct->dicom_patient_id = syd::GetTagValueAsString<0x10,0x20>(dataset);
  dicom_struct->dicom_patient_id = trim(dicom_struct->dicom_patient_id);
  dicom_struct->dicom_patient_birth_date = syd::GetTagValueAsString<0x10,0x30>(dataset);
  dicom_struct->dicom_patient_sex = syd::GetTagValueAsString<0x10,0x40>(dataset);
  return;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomBuilder::UpdateDicomStruct(syd::DicomStruct::pointer dicom_struct,
                                          const gdcm::DataSet & dataset)
{
  dicom_struct->dicom_study_uid = syd::GetTagValueAsString<0x20,0x0d>(dataset);
  dicom_struct->dicom_series_uid = syd::GetTagValueAsString<0x20,0x0e>(dataset);
  auto seq = GetSequence(dataset, 0x3006,0x0010);
  auto item = seq->GetItem(1);
  auto s = syd::GetTagValueAsString<0x20,0x52>(item.GetNestedDataSet());
  dicom_struct->dicom_frame_of_reference_uid = s;
  dicom_struct->dicom_modality = syd::GetTagValueAsString<0x08,0x60>(dataset);
  dicom_struct->dicom_series_description = syd::GetTagValueAsString<0x08,0x103e>(dataset);
  dicom_struct->dicom_study_description = syd::GetTagValueAsString<0x08,0x1030>(dataset);
  //dicom_struct->dicom_study_name = syd::GetTagValueAsString<0x09,0x1010>(dataset);
  dicom_struct->dicom_study_id = syd::GetTagValueAsString<0x20,0x0010>(dataset);
  dicom_struct->dicom_image_id = syd::GetTagValueAsString<0x54,0x0400>(dataset);
  //dicom_struct->dicom_dataset_name = syd::GetTagValueAsString<0x11,0x1012>(dataset);
  dicom_struct->dicom_manufacturer = syd::GetTagValueAsString<0x08,0x0070>(dataset);
  dicom_struct->dicom_manufacturer_model_name = syd::GetTagValueAsString<0x08,0x1090>(dataset);
  dicom_struct->dicom_software_version = syd::GetTagValueAsString<0x18,0x1020>(dataset);

  dicom_struct->dicom_description = dicom_struct->dicom_series_description+" "+
    dicom_struct->dicom_study_description+" "+dicom_struct->dicom_image_id+" "+
    dicom_struct->dicom_dataset_name+" "+dicom_struct->dicom_study_id+" "+
    dicom_struct->dicom_study_name;

  dicom_struct->dicom_station_name = syd::GetTagValueAsString<0x08,0x1010>(dataset);
  dicom_struct->dicom_protocol_name = syd::GetTagValueAsString<0x18,0x1030>(dataset);
  dicom_struct->dicom_structure_set_label = syd::GetTagValueAsString<0x3006,0x0002>(dataset);
  dicom_struct->dicom_structure_set_name = syd::GetTagValueAsString<0x3006,0x0004>(dataset);
  auto date = syd::GetTagValueAsString<0x3006,0x0008>(dataset);
  auto time = syd::GetTagValueAsString<0x3006,0x0009>(dataset);
  dicom_struct->dicom_structure_set_date = ConvertDicomDateToStringDate(date, time);

  auto dicom_file = dicom_struct->dicom_files[0];
  dicom_file->dicom_sop_uid = syd::GetTagValueAsString<0x08,0x18>(dataset);
  dicom_file->dicom_instance_number = 1;

  // Get Structure Set ROI Sequence
  gdcm::Tag tssroisq(0x3006,0x0020);
  if (!dataset.FindDataElement(tssroisq)) {
    LOG(WARNING) << "Problem locating 0x3006,0x0020 - Is this a valid RT Struct file?" << std::endl;
    return;
  }
  auto & ssroisq = dataset.GetDataElement(tssroisq);
  auto roi_seq = ssroisq.GetValueAsSQ();
  for(auto i = 0; i < roi_seq->GetNumberOfItems(); ++i){
    auto & item = roi_seq->GetItem(i+1); // Item starts at 1
    auto & nested_dataset = item.GetNestedDataSet();
    auto name = syd::GetTagValueAsString<0x3006,0x26>(nested_dataset);
    dicom_struct->dicom_roi_names.push_back(name);
  }

  // Set the same size (nullptr)
  dicom_struct->roi_types.resize(dicom_struct->dicom_roi_names.size());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomBuilder::SetDicomPatient(syd::DicomBase::pointer dicom,
                                        syd::Patient::pointer patient)
{
  if (patient == nullptr) patient = syd::FindPatientFromDicomInfo(db, dicom);
  if (patient == nullptr) {
    patient = syd::NewPatientFromDicomInfo(db, dicom);
    LOG(2) << "Create patient " << dicom->patient;
    db->Insert(patient);
  }
  syd::CheckAndSetPatient(dicom, patient);

  // Update patient
  if (update_patient_info_from_file_flag)
    syd::SetPatientInfoFromDicom(dicom, dicom->patient);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomBuilder::InsertDicom()
{
  int n_series = InsertDicomSeries();
  int n_struct = InsertDicomStruct();
  int n_dicomfiles = dicom_series_dicom_files_to_insert.size()+n_struct;

  // Log
  LOG(1) << n_series << " DicomSeries has been added in the db";
  LOG(1) << n_struct << " DicomStruct has been added in the db";
  LOG(1) << n_dicomfiles << " DicomFiles have been added in the db";

  if (nb_of_skip_files != 0) {
    LOG(1) << nb_of_skip_files << " dicom already existing in the db and have been skipped.";
  }
  if (nb_of_skip_copy != 0) {
    LOG(1) << nb_of_skip_copy << " files already existing in the db folder and have not been copied.";
  }

  // Once done, clear vectors
  dicom_series_dicom_files_to_insert.clear();
  dicom_series_to_insert.clear();
  dicom_series_dicom_files_to_insert.clear();
  dicom_files_corresponding_series.clear();
  dicom_series_filenames_to_copy.clear();
  dicom_struct_to_insert.clear();
  dicom_struct_dicom_files_to_insert.clear();
  dicom_struct_filenames_to_copy.clear();
  nb_of_skip_files = 0;
  nb_of_skip_copy = 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::DicomBuilder::InsertDicomSeries()
{
  // Create folder if needed
  for(auto dicom_serie:dicom_series_to_insert)
    syd::CreateDicomFolder(db, dicom_serie);

  // Update the database first to get the File id
  db->Insert(dicom_series_dicom_files_to_insert); // must be before serie
  db->Insert(dicom_series_to_insert);

  // Copy files
  int n = dicom_series_filenames_to_copy.size();
  for(auto i=0; i<n; i++) {
    auto filename = dicom_series_filenames_to_copy[i];
    auto dicom_file = dicom_series_dicom_files_to_insert[i];
    syd::SetDicomFilePathAndFilename(dicom_file,
                                     filename,
                                     dicom_files_corresponding_series[i]);
    bool b = syd::CopyFileToDicomFile(filename, dicom_file);
    if (!b) ++nb_of_skip_copy;
    loadbar(i,n);
  }

  // Update because the filename changed
  db->Update(dicom_series_dicom_files_to_insert);

  return dicom_series_to_insert.size();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::DicomBuilder::InsertDicomStruct()
{
  // Create folder if needed
  for(auto dicom_struct:dicom_struct_to_insert)
    syd::CreateDicomFolder(db, dicom_struct);

  // Update the database first to get the File id
  db->Insert(dicom_struct_dicom_files_to_insert); // must be before serie
  db->Insert(dicom_struct_to_insert);

  // Copy files
  int n = dicom_struct_to_insert.size();
  for(auto i=0; i<n; i++) {
    auto filename = dicom_struct_filenames_to_copy[i];
    auto dicom_file = dicom_struct_dicom_files_to_insert[i];
    syd::SetDicomFilePathAndFilename(dicom_file,
                                     filename,
                                     dicom_struct_to_insert[i]);
    bool b = syd::CopyFileToDicomFile(filename, dicom_file);
    if (!b) ++nb_of_skip_copy;
    loadbar(i,n);
  }

  // Update because the filename changed
  db->Update(dicom_struct_dicom_files_to_insert);

  return n;
}
// --------------------------------------------------------------------
