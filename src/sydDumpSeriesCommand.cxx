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
  for(auto i=1; i<n; i++)
    patterns_.push_back(inputs[i]);
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
  typedef odb::query<Serie> QueryType;
  QueryType q = db_->GetSeriesQueryFromPatterns(patterns_);
  q = (QueryType::patient_id == patient_.id) && q;
  std::vector<Serie> series;
  db_->LoadVector<Serie>(series, q);

  // Sort by acquisition_date
  std::sort(series.begin(), series.end(),
            [&](Serie a, Serie b) { return syd::IsBefore(a.acquisition_date, b.acquisition_date); }  );

  // Now display results
  for(auto i=0; i<series.size(); i++) {
    Serie serie = series[i];
    std::cout << patient_.name << " "
              << patient_.synfrizz_id << " "
              << serie.id << " "
              << serie.acquisition_date << " "
              << serie.reconstruction_date << " "
              << serie.dicom_study_desc << "\t"
              << serie.dicom_series_desc << "\t"
              << serie.dicom_dataset_name << "\t"
              << serie.dicom_image_id << "\t"
              << serie.dicom_instance_number << "\t"
              << std::endl;
  }

  // Cmd line for vv
  if (0) {
    std::cout << "vv ";
    for(auto i=series.begin(); i<series.end(); i++) {
      std::cout << db_->GetFullPath(*i) << " ";
    }
    std::cout << std::endl;
  }

  // Display ids
  for(auto i=series.begin(); i<series.end(); i++) {
    std::cout << i->id << " ";
  }
  std::cout << std::endl;
}
// --------------------------------------------------------------------