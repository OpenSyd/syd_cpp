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
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  sydlog::Log::SQLFlag() = false;
  sydlog::Log::LogLevel() = 3;

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = "test1.db";
  std::string folder = "data";

  // Create StandardDatabase
  std::cout << "Create StandardDatabase " << dbname << std::endl;
  m->Create("StandardDatabase", dbname, folder, true);

  std::cout << std::endl << "Open as generic Database" << std::endl;
  syd::Database * db = m->Open(dbname);
  db->InsertDefaultRecords("all");
  db->Dump(std::cout);

  // Insert (plugin style)
  std::cout << std::endl << "Insert" << std::endl;
  auto patient = db->New("Patient");
  std::vector<std::string> args = {"toto", "1", "50", "XXXXX", "M"};
  patient->Set(args);
  db->Insert(patient);
  std::cout << "Patient  : " << patient << std::endl;
  if (patient->ToString() != "1 1 toto 50 XXXXX M") {
    LOG(FATAL) << "Error insert patient";
  }

  auto injection = db->New("Injection");
  args = {"toto", "Lu-177", "2013-02-03 10:33", "188.3"};
  injection->Set(args);
  db->Insert(injection);
  std::cout << "Injection: " << injection << std::endl;
  if (injection->ToString() != "1 toto Lu-177 2013-02-03 10:33 188.3") {
    LOG(FATAL) << "Error insert injection";
  }

  // Update
  std::cout << std::endl << "Update" << std::endl;
  db->Update(patient, "weight_in_kg", "100.0");
  db->Update(injection, "activity_in_MBq", "666");
  std::cout << "Patient  : " << patient << std::endl;
  std::cout << "Injection: " << injection << std::endl;
  if (patient->ToString() != "1 1 toto 100 XXXXX M") {
    LOG(FATAL) << "Error update patient";
  }
  if (injection->ToString() != "1 toto Lu-177 2013-02-03 10:33 666") {
    LOG(FATAL) << "Error update injection";
  }

  // Query
  std::cout << std::endl << "Query" << std::endl;
  syd::Record::pointer p2;
  db->QueryOne(p2, "Patient", 1);
  if (p2 != patient) { LOG(FATAL) << "Error query patient"; }
  syd::Record::pointer i2;
  db->QueryOne(i2, "Injection", 1);
  if (i2 != injection) { LOG(FATAL) << "Error query injection"; }
  std::cout << "Patient  : " << p2 << std::endl;
  std::cout << "Injection: " << i2 << std::endl;

  // Delete
  std::cout << std::endl << "Delete" << std::endl;
  db->Delete(injection); // first because foreign_keys for patient
  db->Delete(patient);
  int n = db->GetNumberOfElements("Patient");
  if (n != 0) { LOG(FATAL) << "Error delete patient"; }
  n = db->GetNumberOfElements("Injection");
  if (n != 0) { LOG(FATAL) << "Error delete injection"; }
  std::cout << n << std::endl;

  std::cout << "Success." << std::endl;
  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
