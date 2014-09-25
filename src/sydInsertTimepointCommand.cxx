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
#include "sydInsertTimepointCommand.h"
#include "sydImage.h"

// --------------------------------------------------------------------
syd::InsertTimepointCommand::InsertTimepointCommand(std::string db1, std::string db2):DatabaseCommand()
{
  db_ = OpenNewDatabase<ClinicDatabase>(db1);
  tpdb_ = OpenNewDatabase<TimepointsDatabase>(db2);
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertTimepointCommand::InsertTimepointCommand(syd::ClinicDatabase * db1, syd::TimepointsDatabase  * db2):
  db_(db1), tpdb_(db2)
{
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimepointCommand::Initialization()
{
  tpdb_->set_clinic_database(db_);
  ct_selection_patterns_.clear();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertTimepointCommand::~InsertTimepointCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimepointCommand::set_ct_selection_patterns(std::string s)
{
  std::istringstream iss(s);
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(ct_selection_patterns_));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimepointCommand::InsertTimepoint(std::vector<std::string> inputs)
{
  // Convert serie to u long
  std::vector<IdType> ids;
  for(auto i=0; i<inputs.size(); i++) {
    IdType id = toULong(inputs[i]);
    ids.push_back(id);
  }

  // Insert all series
  for(auto i: ids) {
    Serie serie = db_->GetById<Serie>(i);
    InsertTimepoint(serie);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimepointCommand::InsertTimepoint(Serie serie)
{
  // Check modality
  if (serie.modality != "NM") {
    LOG(FATAL) << "Error the serie " << serie.id << " modality is " << serie.modality
               << " while expecting NM";
  }

  // Get patient
  if (db_->GetIfExist<Patient>(odb::query<Patient>::id == serie.patient_id, patient_)) {
    db_->CheckPatient(patient_);
  }
  else {
    LOG(FATAL) << "Error, the patient for this serie (" << serie << ") does not exist";
  }

  // Check folder if does not exist
  std::string path = tpdb_->GetFullPath(patient_);
  if (!OFStandard::dirExists(path.c_str())) {
    LOG(FATAL) << "The directory " << path << " does not exist. Please create.";
  }

  // Create or Update a new Timepoint
  Timepoint timepoint;
  bool b = tpdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::serie_id == serie.id, timepoint);
  if (!b) {  // It does not exist, we create it
    VLOG(1) << "Creating new Timepoint for " << patient_.name << " date " << serie.acquisition_date;
    timepoint.patient_id = patient_.id;
    timepoint.serie_id = serie.id;
    timepoint.number=0;
    tpdb_->Insert(timepoint);
  }
  else {
    VLOG(1) << "Timepoint " << patient_.name << " "
            << timepoint.number << " "
            << serie.acquisition_date << " ("
            << timepoint.time_from_injection_in_hours
            << " hours) already exist, deleting current image and updating.";
    std::string path = tpdb_->GetFullPathSPECT(timepoint);
    syd::DeleteMHDImage(path);
    path = tpdb_->GetFullPathCT(timepoint);
    syd::DeleteMHDImage(path);
  }

  // Set a temporary number (higher than the previous)
  std::vector<Timepoint> timepoints;
  tpdb_->LoadVector<Timepoint>(timepoints, odb::query<Timepoint>::patient_id == patient_.id);
  int max = 0;
  for(auto i=timepoints.begin(); i<timepoints.end(); i++) if (i->number > max) max = i->number;
  timepoint.number = max+1;

  // Update the time
  if (patient_.injection_date == "") {
    LOG(FATAL) << "Injection date for the patient " << patient_.name << " is missing.";
  }
  timepoint.time_from_injection_in_hours = syd::DateDifferenceInHours(serie.acquisition_date, patient_.injection_date);

  // Update field
  tpdb_->Update(timepoint);

  // Convert the dicom to mhd (SPECT)
  std::string dicom_filename = db_->GetFullPath(serie);
  std::string mhd_filename   = tpdb_->GetFullPathSPECT(timepoint);
  VLOG(2) << "Converting SPECT dicom to mhd (" << mhd_filename << ") ...";
  syd::ConvertDicomToImage(dicom_filename, mhd_filename);

  // Retrieve corresponding CT
  std::vector<Serie> series;
  db_->LoadVector<Serie>(series,
                         odb::query<Serie>::dicom_frame_of_reference_uid ==
                         serie.dicom_frame_of_reference_uid &&
                         odb::query<Serie>::modality == "CT");

  // Check how many series we found
  if (series.size() == 0) {
    LOG(FATAL) << "Error could not find corresponding ct series with dicom_frame_of_reference_uid = " << serie.dicom_frame_of_reference_uid;
  }

  // If we found several series with the dicom_frame_of_reference_uid
  if (series.size() > 1) {
    if (ct_selection_patterns_.size() == 0) {
      LOG(FATAL) << "Error we found " << series.size() << " series with the correct dicom_frame_of_reference_uid."
                 << std::endl
                 << "Please use the ct_pattern option to select the one you want.";
    }
    VLOG(3) << "We found " << series.size() << " serie(s). Selection with the ct_pattern option...";
    typedef odb::query<Serie> QueryType;
    QueryType q = db_->GetSeriesQueryFromPatterns(ct_selection_patterns_);
    q = (QueryType::dicom_frame_of_reference_uid == serie.dicom_frame_of_reference_uid &&
         QueryType::modality == "CT" &&
         QueryType::patient_id == patient_.id) && q;
    series.clear();
    db_->LoadVector<Serie>(series, q);
  }

  if (series.size() != 1) {
    LOG(FATAL) << "Error we found " << series.size() << " serie(s) with the dicom_frame_of_reference_uid and ct_pattern for this spect.";
  }

  Serie ct_serie = series[0];
  std::string dicom_path = db_->GetFullPath(ct_serie);
  std::string ct_mhd_filename   = tpdb_->GetFullPathCT(timepoint);

  // read all dicom in the folder
  typedef itk::Image<signed short, 3> ImageType;
  typedef itk::ImageSeriesReader<ImageType> ReaderType;
  typedef itk::GDCMImageIO ImageIOType;
  typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
  inputNames->SetInputDirectory(dicom_path);
  const ReaderType::FileNamesContainer & filenames = inputNames->GetInputFileNames();
  VLOG(2) << "Converting CT dicom (with " << filenames.size() << " files) to mhd (" << ct_mhd_filename << ") ...";
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetImageIO( gdcmIO );
  reader->SetFileNames( filenames );
  try { reader->Update(); }
  catch (itk::ExceptionObject &excp) {
    std::cerr << excp << std::endl;
    LOG(FATAL) << "Error while reading the dicom serie in " << dicom_path << " ";
  }

  // convert to mhd
  ImageType::Pointer ct = reader->GetOutput();
  syd::WriteImage<ImageType>(ct, ct_mhd_filename);

  // Find time order according to existing timepoint
  tpdb_->UpdateAllTimepointNumbers(patient_.id);
}
// --------------------------------------------------------------------
