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
#include "sydAddTimePointCommand.h"

// --------------------------------------------------------------------
syd::AddTimePointCommand::AddTimePointCommand():DatabaseCommand()
{
  db_ = NULL;
  tpdb_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::AddTimePointCommand::~AddTimePointCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::AddTimePointCommand::AddDatabase(syd::Database * d)
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
void syd::AddTimePointCommand::SetArgs(char ** inputs, int n)
{
  if (n != 2) {
    LOG(FATAL) << "Two parameters are needed : <patient> <dcm_filename>, but you provide "
               << n << " parameter(s)";
  }
  patient_name_ = inputs[0];
  filename_ = inputs[1];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::AddTimePointCommand::Run()
{
  // Check database
  if (db_ == NULL) {
    LOG(FATAL) << "A ClinicalTrialDatabase is needed in AddTimePointCommand. Aborting.";
  }

  if (tpdb_ == NULL) {
    LOG(FATAL) << "A TimePointsDatabase is needed in AddTimePointCommand. Aborting.";
  }

  // Get the new patient
  if (db_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name_, patient_)) {
    db_->CheckPatient(patient_);
  }
  else {
    LOG(FATAL) << "Error, the patient " << patient_name_ << " does not exist";
  }

  Run(filename_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::AddTimePointCommand::Run(std::string filename)
{
  DD(filename);

  // open dcm files
  DcmFileFormat dfile;
  syd::OpenDicomFile(filename, false, dfile);
  DcmObject *dset = dfile.getDataset();

  // get uid, retrive in the db (check)
  std::string SOPInstanceUID = GetTagValue(dset, "SOPInstanceUID");
  std::string modality = GetTagValue(dset, "Modality");
  if ((modality != "OT") && (modality != "NM")) {
    LOG(FATAL) << "Error the dicom is not OT or NM modality, but read " << modality;
  }
  DD(SOPInstanceUID);
  Serie serie;
  bool b = db_->GetIfExist<Serie>(odb::query<Serie>::dicom_uid == SOPInstanceUID, serie);
  if (!b) {
    LOG(FATAL) << "Error, the serie with uid = " << SOPInstanceUID << " does not exist in the db.";
  }
  DD(serie);

  // Create or Update a new TimePoint
  TimePoint timepoint;
  b = tpdb_->GetIfExist<TimePoint>(odb::query<TimePoint>::serie_id == serie.id, timepoint);
  if (!b) {  // the timepoint already exist
    VLOG(1) << "Creating new TimePoint";
    timepoint.patient_id = patient_.id;
    timepoint.serie_id = serie.id;
    tpdb_->Insert(timepoint);
  }
  else {
    VLOG(1) << "TimePoint already exist, updating.";
    // Check patient
    if (patient_.id != timepoint.patient_id) {
      LOG(FATAL) << "The dicom is linked to a different patient in the db. The id in the db is "
                 << timepoint.patient_id  << " while the patient you ask (" << patient_name_
                 << ") has id = " << patient_.id;
    }
    // Check serie
    if (serie.id != timepoint.serie_id) {
      LOG(FATAL) << "The dicom is linked to a different serie in the db. The id in the db is "
                 << timepoint.serie_id  << " while the serie has id = " << serie.id;
    }
  }

  // retrive associated ct -> check
  //(0020,0052) UI [1.2.840.113619.2.280.2.1.12022013121509909.1675362126] #  54, 1 FrameOfReferenceUID
  std::string FrameOfReferenceUID = GetTagValue(dset, "FrameOfReferenceUID");
  DD(FrameOfReferenceUID);
  //  b = db_->GetIfExist<Serie>(
  std::vector<Serie> series;
  db_->LoadVector<Serie>(series,
                         odb::query<Serie>::dicom_frame_of_reference_uid == FrameOfReferenceUID &&
                         odb::query<Serie>::modality == "CT");
  DD(series.size());
  DD(series[0]);


  // get date
  std::string AcquisitionTime = GetTagValue(dset, "AcquisitionTime");
  std::string AcquisitionDate = GetTagValue(dset, "AcquisitionDate");
  std::string date = GetDate(AcquisitionDate, AcquisitionTime);
  timepoint.acquisition_date = date;

  // convert mhd clitk ?


  // Update DB
  //  tpdb_->Update(timepoint);


}
// --------------------------------------------------------------------
