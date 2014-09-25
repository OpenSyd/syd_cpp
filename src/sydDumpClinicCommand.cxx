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
#include "sydDumpClinicCommand.h"

// --------------------------------------------------------------------
syd::DumpClinicCommand::DumpClinicCommand(std::string db):DatabaseCommand()
{
  db_ = OpenNewDatabase<ClinicDatabase>(db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DumpClinicCommand::DumpClinicCommand(syd::ClinicDatabase * db):DatabaseCommand(), db_(db)
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DumpClinicCommand::~DumpClinicCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DumpClinicCommand::Dump(std::string cmd,
                                  std::string patient_name,
                                  std::vector<std::string> & patterns)
{
  // Get the patients
  std::vector<Patient> patients;
  db_->GetPatientsByName(patient_name, patients);

  // cmd to lower case
  std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

  // Warning if cmd is 'patient' because patterns is not used
  if (cmd.find("patient") != std::string::npos) {
    if (patterns.size() != 0) {
      std::string p;
      for(auto i=patterns.begin(); i != patterns.end(); i++) p = p+*i+" ";
      LOG(WARNING) << "The string patterns you provide ('" << p << "') are ignored.";
    }
  }

  // Check cmd
  if ((cmd.find("serie") == std::string::npos) &&
      (cmd.find("patient") == std::string::npos)) {
    LOG(FATAL) << "Error DumpClinic type must be 'serie' or 'patient'";
  }

  // cmd = patient : loop over patient to display
  if (cmd.find("patient")!= std::string::npos)
    for(auto i:patients) std::cout << db_->Print(i,2) << std::endl;

  // cmd = serie : loop over all series for all patients
  if (cmd.find("serie")!= std::string::npos) {
    for(auto patient:patients) {
      typedef odb::query<Serie> QueryType;
      QueryType q = db_->GetSeriesQueryFromPatterns(patterns);
      q = (QueryType::patient_id == patient.id) && q;
      std::vector<Serie> series;
      db_->LoadVector<Serie>(series, q);

      // Sort by acquisition_date
      std::sort(series.begin(), series.end(),
                [&](Serie a, Serie b) { return syd::IsBefore(a.acquisition_date, b.acquisition_date); }  );

      // Print all series
      for(auto i:series) std::cout << db_->Print(i) << std::endl;

      // Print serie ids
      std::cout << patient.name << " " << patient.synfrizz_id << " (total " << series.size() << ") ";
      for(auto i: series) std::cout << i.id << " ";
      std::cout << std::endl;
    }
  }
}
// --------------------------------------------------------------------
