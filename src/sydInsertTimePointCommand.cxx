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

  // Get the new patient
  if (db_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name_, patient_)) {
    db_->CheckPatient(patient_);
  }
  else {
    LOG(FATAL) << "Error, the patient " << patient_name_ << " does not exist";
  }

  // Create folder if does not exist

  DD("TODO"); // FIXME

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
  DD(serie);

  // Check modality
  if (serie.modality != "NM") {
    LOG(FATAL) << "Error the serie " << serie.id << " modality is " << serie.modality
               << " while expecting NM";
  }

  // Create or Update a new TimePoint
  TimePoint timepoint;
  bool b = tpdb_->GetIfExist<TimePoint>(odb::query<TimePoint>::serie_id == serie.id, timepoint);
  if (!b) {  // the timepoint already exist
    VLOG(1) << "Creating new TimePoint";
    //timepoint.patient_id = patient_.id;
    timepoint.serie_id = serie.id;
    tpdb_->Insert(timepoint);
  }
  else {
    VLOG(1) << "TimePoint already exist, updating.";
    // Check patient
    // if (patient_.id != timepoint.patient_id) {
    //   LOG(FATAL) << "The dicom is linked to a different patient in the db. The id in the db is "
    //              << timepoint.patient_id  << " while the patient you ask (" << patient_name_
    //              << ") has id = " << patient_.id;
    // }
  }

  // Retrieve corresponding CT
  std::vector<Serie> series;
  db_->LoadVector<Serie>(series,
                         odb::query<Serie>::dicom_frame_of_reference_uid ==
                         serie.dicom_frame_of_reference_uid &&
                         odb::query<Serie>::modality == "CT");
  DD(series.size());
  DD(series[0]);

  // update field
  //timepoint.acquisition_date = serie.acquisition_date;


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
  DD(spect->GetOrigin());
  DD(spect->GetSpacing());

  // Open dicom header
  DcmFileFormat dfile;
  b = syd::OpenDicomFile(filename.c_str(), true, dfile);
  DcmObject *dset = dfile.getDataset();
  if (!b) {
    LOG(FATAL) << "Could not open the file " << filename;
  }

  // Check nb of slices
  ImageType::SizeType size = spect->GetLargestPossibleRegion().GetSize();
  DD(size);
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
  DD(s);
  if (s<0) {
    DD("correction");
    // change spacing z
    ImageType::SpacingType spacing = spect->GetSpacing();
    spacing[2] = -s;
    spect->SetSpacing(spacing);
    // Direction
    ImageType::DirectionType direction = spect->GetDirection();
    DD(direction);
    direction.Fill(0.0);
    direction(0,0) = 1; direction(1,1) = 1; direction(2,2) = -1;
    DD(direction);
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
  DD(origin);
  spect->SetOrigin(origin);


  // Write mhd
  std::string mhd_filename = "toto.mhd";
  typedef itk::ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(mhd_filename.c_str());
  writer->SetInput(spect);
  try { writer->Update(); }
  catch(itk::ExceptionObject & err) {
    LOG(FATAL) << "Error while writing image [" << mhd_filename << "]";
  }


  // Check pixel size (because bug sometimes)
  // int sp = atoi(GetTagValue
  // PixelSpacing
  //   SpacingBetweenSlices
  //    NumberOfSlices

  // Check
  //  check that another serie with same acqui date exist

  // Update DB
  tpdb_->Update(timepoint);
  DD(timepoint);

}
// --------------------------------------------------------------------
