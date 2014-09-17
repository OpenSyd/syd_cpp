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
#include "sydDumpSeriesCommand.h"

// --------------------------------------------------------------------
syd::DumpSeriesCommand::DumpSeriesCommand():DatabaseCommand()
{
  db_ = NULL;
  patient_name_ = "noname";

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DumpSeriesCommand::~DumpSeriesCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DumpSeriesCommand::SetArgs(char ** inputs, int n)
{
  if (n < 2) { // FIXME
    LOG(FATAL) << "Two parameters are needed TODO, but you provide "
               << n << " parameter(s)";
  }
  patient_name_ = inputs[0];
  pattern_ = inputs[1];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DumpSeriesCommand::AddDatabase(syd::Database * d)
{
  if (databases_.size() != 0) {
    LOG(FATAL) << "DumpSeriesCommand::AddDatabase: already a db.";
  }
  DatabaseCommand::AddDatabase(d);
  db_ = static_cast<ClinicalTrialDatabase*>(d);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DumpSeriesCommand::Run()
{
  // Check database
  if (db_ == NULL) {
    LOG(FATAL) << "A (single) database of type ClinicalTrialDatabase "
               << "is needed in DumpSeriesCommand. Aborting.";
  }

  // Get the new patient
  if (db_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name_, patient_)) {
    db_->CheckPatient(patient_);
  }
  else {
    LOG(FATAL) << "Error, the patient " << patient_name_ << " does not exist";
  }

  // Create the query to retrieve the series
  pattern_ = "%"+pattern_+"%";
  typedef odb::query<Serie> QueryType;
  QueryType q;
  q = (QueryType::dicom_dataset_name.like(pattern_) ||
       QueryType::dicom_image_id.like(pattern_) ||
       QueryType::modality.like(pattern_) ||
       QueryType::dicom_series_desc.like(pattern_) ||
       QueryType::dicom_study_desc.like(pattern_));
  q = (QueryType::patient_id == patient_.id) && q;
  std::vector<Serie> series;
  db_->LoadVector<Serie>(series, q);

  // Now display results
  for(auto i=series.begin(); i<series.end(); i++) {
    std::cout << i->id << " "
              << i->acquisition_date << " "
              << i->reconstruction_date << " "
              << i->dicom_study_desc << "\t"
              << i->dicom_series_desc << "\t"
              << i->dicom_dataset_name << "\t"
              << i->dicom_image_id << "\t"
              << i->dicom_instance_number << "\t"
              << std::endl;
  }

}
// --------------------------------------------------------------------
