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
#include "sydStandardDatabase.h"
#include "sydDicomSerieBuilder.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  sydlog::Log::SQLFlag() = false;
  sydlog::Log::LogLevel() = 3;

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = "test3.db";
  std::string folder = "data3";

  // Create StandardDatabase
  std::cout << "Current path is " << fs::current_path() << std::endl;
  std::cout << "Create StandardDatabase " << dbname << std::endl;
  m->Create("StandardDatabase", dbname, folder, true);

  std::cout << std::endl << "Open as StandardDatabase" << std::endl;
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(dbname);
  db->InsertDefaultRecords("all");

  // Insert patient
  syd::Patient::pointer patient;
  db->New(patient);
  std::vector<std::string> args = {"toto", "1", "50", "XXXXX", "M"};
  patient->Set(args);
  db->Insert(patient);

  // Build files list
  std::vector<std::string> files;
  syd::SearchAndAddFilesInFolder(files, "dicom", true);
  st::sort(files.begin(), files.end());
  // need to sort because orer of files may vary from system to system

  // insert dicom from folder
  syd::DicomSerieBuilder builder(db);
  builder.SetPatient(patient);
  builder.SetForcePatientFlag(true);
  for(auto f:files) builder.SearchDicomInFile(f);
  builder.InsertDicomSeries();

  // Create output
  syd::DicomSerie::vector dicoms;
  db->Query(dicoms);
  db->Sort(dicoms);
  std::stringstream b;
  for(auto d:dicoms) b << d << std::endl;

  // Read ref output
  // generated with sydFind --db test3.db dicomserie -f raw > test3_ref.txt
  std::ifstream is("test3_ref.txt");
  std::stringstream ref;
  ref << is.rdbuf();

  // Check
  if (ref.str() != b.str()) {
    DD(ref.str());
    DD(b.str());
    LOG(FATAL) << "Error inserting dicom.";
  }

  std::cout << "Success." << std::endl;
  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
