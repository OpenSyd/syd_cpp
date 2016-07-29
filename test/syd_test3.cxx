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
#include "syd_test3_ggo.h"
#include "sydCommonGengetopt.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydDicomSerieBuilder.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(syd_test3, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // database names
  std::string dbname = "test3.db";
  std::string folder = "data3";
  std::string ref_dbname = "test3_ref.db";
  std::string ref_folder = "data3_ref";

  // Create StandardDatabase
  std::cout << "Current path is " << fs::current_path() << std::endl;
  std::cout << "Create StandardDatabase " << dbname << std::endl;
  m->Create("StandardDatabase", dbname, folder, true);

  std::cout << "Open as StandardDatabase" << std::endl;
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
  std::sort(files.begin(), files.end());
  // need to sort because order of files may vary from system to system

  // insert dicom from folder
  syd::DicomSerieBuilder builder(db);
  builder.SetPatient(patient);
  builder.SetForcePatientFlag(true);
  for(auto f:files) builder.SearchDicomInFile(f);
  builder.InsertDicomSeries();

  // If needed create reference db
  if (args_info.create_ref_flag) {
    LOG(0) << "Create reference db (not the associated folder)";
    db->Copy(ref_dbname);
  }

  // Check
  auto ref_db = m->Open<syd::StandardDatabase>(ref_dbname);
  syd::DicomSerie::vector dicom_series;
  syd::DicomSerie::vector ref_dicom_series;
  db->Query(dicom_series);
  ref_db->Query(ref_dicom_series);
  syd::Record::vector v;
  for(auto d:dicom_series) v.push_back(d);
  db->Dump(v, "default", std::cout);

  for(auto i=0; i< dicom_series.size(); i++) {
    if (dicom_series[i] != ref_dicom_series[i]) {
      LOG(FATAL) << "Error DicomSerie different " << std::endl
                 << dicom_series[i] << std::endl
                 << ref_dicom_series[i] << std::endl;
    }
    // Do not check files here
  }

  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
