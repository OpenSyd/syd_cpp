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
#include "sydDumpStudyCommand.h"

// --------------------------------------------------------------------
syd::DumpStudyCommand::DumpStudyCommand(std::string db1, std::string db2):DatabaseCommand()
{
  cdb_ = OpenNewDatabase<ClinicDatabase>(db1);
  sdb_ = OpenNewDatabase<StudyDatabase>(db2);
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DumpStudyCommand::DumpStudyCommand(syd::ClinicDatabase * db1, syd::StudyDatabase  * db2):
  cdb_(db1), sdb_(db2)
{
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DumpStudyCommand::Initialization()
{
  sdb_->set_clinic_database(cdb_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DumpStudyCommand::~DumpStudyCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DumpStudyCommand::Dump(std::string patient_name)
{
  // Get the patients
  std::vector<Patient> patients;
  cdb_->GetPatientsByName(patient_name, patients);

  // Dump all patients
  for(auto i:patients) std::cout << sdb_->Print(i) << std::endl;
}
// --------------------------------------------------------------------
