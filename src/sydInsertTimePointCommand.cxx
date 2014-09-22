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

// --------------------------------------------------------------------
syd::InsertTimePointCommand::InsertTimePointCommand():DatabaseCommand()
{
  db_ = NULL;
  tpdb_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertTimePointCommand::~InsertTimePointCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimePointCommand::AddDatabase(syd::Database * d)
{
  if (databases_.size() == 0) { // first db
    DatabaseCommand::AddDatabase(d);
    tpdb_ = static_cast<TimePointsDatabase*>(d);
  }
  else {
    if (databases_.size() == 1) { // second db
      DatabaseCommand::AddDatabase(d);
      db_ = static_cast<ClinicalTrialDatabase*>(d);
    }
    else {
      LOG(FATAL) << "TimePointsDatabase::AddDatabase error. First provide TimePointsDatabase then ClinicalTrialDatabase.";
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertTimePointCommand::SetArgs(char ** inputs, int n)
{
  if (n < 2) {
    LOG(FATAL) << "At least two parameters are needed : <patient> <series ids>, but you provide "
               << n << " parameter(s)";
  }
  patient_name_ = inputs[0];
  for(auto i=1; i<n; i++) {
    IdType id = toULong(inputs[i]);
    DD(id);
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

  if (db_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name_, patient_)) {
    db_->CheckPatient(patient_);
  }
  else {
    LOG(FATAL) << "Error, the patient " << patient_name_ << " does not exist";
  }

  // Create folder if does not exist
  std::string path = tpdb_->GetFullPath(patient_);
  if (!OFStandard::dirExists(path.c_str())) {
    LOG(FATAL) << "The directory " << path << " does not exist. Please create.";
  }

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
  DD("------------------------------");
  DD(serie);

  // Check modality
  if (serie.modality != "NM") {
    LOG(FATAL) << "Error the serie " << serie.id << " modality is " << serie.modality
               << " while expecting NM";
  }

  // Create or Update a new TimePoint
  TimePoint timepoint;
  bool b = tpdb_->GetIfExist<TimePoint>(odb::query<TimePoint>::serie_id == serie.id, timepoint);
  if (!b) {  // It does not exist, we create it
    VLOG(1) << "Creating new TimePoint";
    timepoint.patient_id = patient_.id;
    timepoint.serie_id = serie.id;
    timepoint.number=0;
    tpdb_->Insert(timepoint);
  }
  else {
    VLOG(1) << "TimePoint already exist, updating.";
  }

  // temporary set a number
  std::vector<TimePoint> timepoints;
  tpdb_->LoadVector<TimePoint>(timepoints, odb::query<TimePoint>::patient_id == patient_.id);
  int max = 0;
  for(auto i=timepoints.begin(); i<timepoints.end(); i++) if (i->number > max) max = i->number;
  DD(max);
  timepoint.number = max+1;

  // Re get the timepoint because UpdateAllTimePointNumbers may have change data
  // tpdb_->GetIfExist<TimePoint>(odb::query<TimePoint>::serie_id == serie.id, timepoint);

  // Update the time
  if (patient_.injection_date == "") {
    LOG(FATAL) << "Injection date for the patient " << patient_.name << " is missing.";
  }
  timepoint.time_from_injection_in_hours = syd::DateDifferenceInHours(serie.acquisition_date, patient_.injection_date);
  DD(timepoint);

  // update field
  //timepoint.acquisition_date = serie.acquisition_date;
  //  timepoint.spect_mhd = "toto.mhd";
  //  DD(timepoint);
  tpdb_->Update(timepoint);
  DD(timepoint);

  // Retrieve corresponding CT
  std::vector<Serie> series;
  db_->LoadVector<Serie>(series,
                         odb::query<Serie>::dicom_frame_of_reference_uid ==
                         serie.dicom_frame_of_reference_uid &&
                         odb::query<Serie>::modality == "CT");
  DD(series.size());
  DD(series[0]);

  // convert mhd clitk ?
  std::string filename = db_->GetFullPath(serie);
  DD(filename);
  //FIXME to put in a function
  typedef itk::Image<float, 3> ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filename.c_str());
  try { reader->Update(); }
  catch(itk::ExceptionObject & err) {
    LOG(FATAL) << "Error while reading image [" << filename << "]";
  }
  ImageType::Pointer spect = reader->GetOutput();

  // Open dicom header
  DcmFileFormat dfile;
  b = syd::OpenDicomFile(filename.c_str(), true, dfile);
  DcmObject *dset = dfile.getDataset();
  if (!b) {
    LOG(FATAL) << "Could not open the file " << filename;
  }

  // Check nb of slices
  ImageType::SizeType size = spect->GetLargestPossibleRegion().GetSize();
  ushort nbslices = GetTagValueUShort(dset, "NumberOfSlices");
  if (nbslices != size[2]) {
    LOG(FATAL) << "Error image spacing is " << size
               << " while in the dicom NumberOfSlices = " << nbslices;
  }

  // Remove meta information (if not : garbage in the mhd)
  itk::MetaDataDictionary d;
  spect->SetMetaDataDictionary(d);

  // Correct for negative SpacingBetweenSlices
  double s = GetTagValueDouble(dset, "SpacingBetweenSlices");
  // change spacing z
  ImageType::SpacingType spacing = spect->GetSpacing();
  if (s<0) spacing[2] = -s;
  else spacing[2] = s;
  spect->SetSpacing(spacing);
  // Direction
  if (s<0) {
    ImageType::DirectionType direction = spect->GetDirection();
    direction.Fill(0.0);
    direction(0,0) = 1; direction(1,1) = 1; direction(2,2) = -1;
    spect->SetDirection(direction);
  }

  // Offset
  std::string ImagePositionPatient = GetTagValueString(dset, "ImagePositionPatient");
  if (ImagePositionPatient == "") {
    LOG(FATAL) << "Error while reading tag ImagePositionPatient in the dicom " << filename;
  }
  int n = ImagePositionPatient.find("\\");
  std::string sx = ImagePositionPatient.substr(0,n);
  ImagePositionPatient = ImagePositionPatient.substr(n+1,ImagePositionPatient.size());
  n = ImagePositionPatient.find("\\");
  std::string sy = ImagePositionPatient.substr(0,n);
  std::string sz = ImagePositionPatient.substr(n+1,ImagePositionPatient.size());
  ImageType::PointType origin;
  origin[0] = toDouble(sx);
  origin[1] = toDouble(sy);
  origin[2] = toDouble(sz);
  spect->SetOrigin(origin);


  // Write mhd
  std::string mhd_filename = tpdb_->GetFullPathSPECT(timepoint);//"toto.mhd";
  DD(mhd_filename);
  // Check if already exsit (it should not !)
  if (OFStandard::fileExists(mhd_filename.c_str())) {
    LOG(FATAL) << "Error the file " << mhd_filename << " already exist, I could not create the temporay image.";
  }

  typedef itk::ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(mhd_filename.c_str());
  writer->SetInput(spect);
  try { writer->Update(); }
  catch(itk::ExceptionObject & err) {
    LOG(FATAL) << "Error while writing image [" << mhd_filename << "]";
  }

  // Check
  //  check that another serie with same acqui date exist
  // Find time order according to existing timepoint
  tpdb_->UpdateAllTimePointNumbers(patient_.id);

  DD(timepoint);
}
// --------------------------------------------------------------------
