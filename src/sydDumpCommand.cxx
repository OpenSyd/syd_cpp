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
#include "sydDumpCommand.h"

// --------------------------------------------------------------------
syd::DumpCommand::DumpCommand():DatabaseCommand()
{
  db_ = NULL;
  patient_name_ = "noname";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DumpCommand::~DumpCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DumpCommand::OpenCommandDatabases()
{
  // Open the ones we want
  db_ = new syd::ClinicDatabase();
  db_->OpenDatabase(get_db_filename("Clinical"), get_db_folder("Clinical"));

  tpdb_ = new syd::TimePointsDatabase();
  tpdb_->OpenDatabase(get_db_filename("TimePoints"), get_db_folder("TimePoints"));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DumpCommand::SetArgs(char ** inputs, int n)
{
  if (n == 1) patient_name_ = "all";
  if (n < 1) {
    LOG(FATAL) << "At least 2 parameters are needed, but you provide "
               << n << " parameter(s)";
  }
  dump_type_ = inputs[0];
  std::transform(dump_type_.begin(), dump_type_.end(), dump_type_.begin(), ::tolower);
  if (n > 1) patient_name_ = inputs[1];
  for(auto i=2; i<n; i++)
    patterns_.push_back(inputs[i]);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DumpCommand::Run()
{
  // Check database
  if (db_ == NULL) {
    LOG(FATAL) << "Error in DumpCommand, could not find a ClinicDatabase.";
  }

  // Consider the patients
  std::vector<Patient> patients;
  if (patient_name_ == "all") {
    db_->LoadVector<Patient>(patients);
    // Sort by acquisition_date
    std::sort(patients.begin(), patients.end(),
              [&](Patient a, Patient b) { return syd::IsBefore(a.injection_date, b.injection_date); }  );
  }
  else {
    // Get the patient
    Patient patient;
    if (!db_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name_, patient)) {
      LOG(FATAL) << "Error, the patient " << patient_name_ << " does not exist";
    }
    patients.push_back(patient);
  }

  if (patients.size() == 0) {
    LOG(FATAL) << "Error not patient found with '" << patient_name_ << "'.";
  }

  if (dump_type_.find("patient") != std::string::npos) {
    if (patterns_.size() != 0) {
      std::string p;
      for(auto i=patterns_.begin(); i != patterns_.end(); i++) p = p+*i+" ";
      LOG(WARNING) << "The string patterns you provide ('" << p << "') are ignored.";
    }
  }

  // Check dump_type_
  if ((dump_type_.find("serie") == std::string::npos) &&
      (dump_type_.find("patient") == std::string::npos) &&
      (dump_type_.find("timepoint") == std::string::npos)) {
    LOG(FATAL) << "Error Dump type must be 'serie' or 'patient' or 'timepoint'";
  }

  for(auto i=patients.begin(); i != patients.end(); i++) {
    //db_->CheckPatient(*i);
    if (dump_type_.find("serie") != std::string::npos) DumpSeries(*i);
    if (dump_type_.find("patient") != std::string::npos) DumpPatients(*i);
    if (dump_type_.find("timepoint") != std::string::npos) DumpTimePoints(*i);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DumpCommand::DumpTimePoints(Patient patient)
{
  typedef odb::query<TimePoint> QueryType;
  std::vector<TimePoint> timepoints;
  tpdb_->LoadVector<TimePoint>(timepoints, QueryType::patient_id == patient.id);

  std::cout << patient.name << " "
            << patient.synfrizz_id << "\t"
            << patient.injection_date << "\t"
            << timepoints.size() << "\t";
  for(auto i=timepoints.begin(); i != timepoints.end(); i++) {
    std::cout << i->time_from_injection_in_hours << " ";
  }
  std::cout << std::endl;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DumpCommand::DumpPatients(Patient patient)
{
  // (pattern is ignored)
  std::cout << patient.name << " "
            << patient.synfrizz_id << "\t"
            << patient.injection_date << "\t"
            << patient.injected_quantity_in_MBq << " MBq\t";

  // Get the number of images
  typedef odb::query<Serie> QueryType;
  std::vector<Serie> series;
  db_->LoadVector<Serie>(series, QueryType::patient_id == patient.id);
  int n = series.size();
  int nb_ct = 0;
  int nb_nm = 0;
  for(auto i=series.begin(); i != series.end(); i++) {
    if (i->modality == "CT") nb_ct++;
    else nb_nm++;
  }
  std::cout << nb_ct << " CTs \t" << nb_nm << " NMs" << std::endl;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DumpCommand::DumpSeries(Patient patient)
{
  // Create the query to retrieve the series
  typedef odb::query<Serie> QueryType;
  QueryType q = db_->GetSeriesQueryFromPatterns(patterns_);
  q = (QueryType::patient_id == patient.id) && q;
  std::vector<Serie> series;
  db_->LoadVector<Serie>(series, q);

  // Sort by acquisition_date
  std::sort(series.begin(), series.end(),
            [&](Serie a, Serie b) { return syd::IsBefore(a.acquisition_date, b.acquisition_date); }  );

  // Now display results
  for(auto i=0; i<series.size(); i++) {
    Serie serie = series[i];
    std::cout << patient.name << " "
              << patient.synfrizz_id << " "
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
  //  if (series.size() > 0)
  std::cout << patient.name << " " << patient.synfrizz_id << " (total " << series.size() << ") ";
  for(auto i=series.begin(); i<series.end(); i++) {
    std::cout << i->id << " ";
  }
  //  if (series.size() > 0)
 std::cout << std::endl;
}
// --------------------------------------------------------------------
