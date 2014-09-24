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
#include "sydInsertTimePointCommand.h"
#include "sydImage.h"

// --------------------------------------------------------------------
syd::InsertTimePointCommand::InsertTimePointCommand():DatabaseCommand()
{
  db_ = NULL;
  tpdb_ = NULL;
  ct_selection_patterns_.clear();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertTimePointCommand::~InsertTimePointCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimePointCommand::OpenCommandDatabases()
{
  // Open the ones we want
  db_ = new syd::ClinicalTrialDatabase();
  db_->OpenDatabase(get_db_filename("Clinical"), get_db_folder("Clinical"));

  tpdb_ = new syd::TimePointsDatabase();
  tpdb_->OpenDatabase(get_db_filename("TimePoints"), get_db_folder("TimePoints"));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimePointCommand::set_ct_selection_patterns(std::string s)
{
  std::istringstream iss(s);
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(ct_selection_patterns_));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimePointCommand::SetArgs(char ** inputs, int n)
{
  if (n < 1) {
    LOG(FATAL) << "At least 1 parameters is needed, but you provide "
               << n << " parameter(s)";
  }
  //  patient_name_ = inputs[0];
  for(auto i=0; i<n; i++) {
    IdType id = toULong(inputs[i]);
    serie_ids_.push_back(id);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimePointCommand::Run()
{
  // Check database
  if (db_ == NULL) {
    LOG(FATAL) << "A ClinicalTrialDatabase is needed in InsertTimePointCommand. Aborting.";
  }

  if (tpdb_ == NULL) {
    LOG(FATAL) << "A TimePointsDatabase is needed in InsertTimePointCommand. Aborting.";
  }

  // Set DB pointer
  tpdb_->set_clinicaltrial_database(db_);

  // if (db_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name_, patient_)) {
  //   db_->CheckPatient(patient_);
  // }
  // else {
  //   LOG(FATAL) << "Error, the patient " << patient_name_ << " does not exist";
  // }

  // // Create folder if does not exist
  // std::string path = tpdb_->GetFullPath(patient_);
  // if (!OFStandard::dirExists(path.c_str())) {
  //   LOG(FATAL) << "The directory " << path << " does not exist. Please create.";
  // }

  // Insert all series
  for(auto i=serie_ids_.begin(); i<serie_ids_.end(); i++)  {
    Serie serie = db_->GetById<Serie>(*i);
    Run(serie);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimePointCommand::Run(Serie serie)
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

  // Create or Update a new TimePoint
  TimePoint timepoint;
  bool b = tpdb_->GetIfExist<TimePoint>(odb::query<TimePoint>::serie_id == serie.id, timepoint);
  if (!b) {  // It does not exist, we create it
    VLOG(1) << "Creating new TimePoint for date " << serie.acquisition_date;
    timepoint.patient_id = patient_.id;
    timepoint.serie_id = serie.id;
    timepoint.number=0;
    tpdb_->Insert(timepoint);
  }
  else {
    VLOG(1) << "TimePoint " << timepoint.number << " "
            << serie.acquisition_date << " ("
            << timepoint.time_from_injection_in_hours
            << " hours) already exist, deleting current image and updating.";
    std::string path = tpdb_->GetFullPathSPECT(timepoint);
    syd::DeleteMHDImage(path);
    path = tpdb_->GetFullPathCT(timepoint);
    syd::DeleteMHDImage(path);
  }

  // Set a temporary number (higher than the previous)
  std::vector<TimePoint> timepoints;
  tpdb_->LoadVector<TimePoint>(timepoints, odb::query<TimePoint>::patient_id == patient_.id);
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
  tpdb_->UpdateAllTimePointNumbers(patient_.id);
}
// --------------------------------------------------------------------
