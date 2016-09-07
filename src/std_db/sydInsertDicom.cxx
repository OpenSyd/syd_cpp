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
#include "sydInsertDicom_ggo.h"
#include "sydDicomSerieBuilder.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertDicom, 2);

  // Load plugin and get the database
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the patient
  std::string name = args_info.inputs[0];
  auto patient = db->FindPatient(name);

  // Get the list of folders to look for
  std::vector<std::string> files_folders;
  for(auto i=1; i<args_info.inputs_num; i++) files_folders.push_back(args_info.inputs[i]);

  // Get the whole list of files
  std::vector<std::string> files;
  for(auto f:files_folders) {
    fs::path path(f);
    if (fs::is_directory(path)) {
      LOG(2) << "Search for dicom in " << f;
      syd::SearchAndAddFilesInFolder(files, f, true);
    }
    else files.push_back(f);
  }
  LOG(1) << "Searching for dicom in " << files.size() << " files ...";

  // Dicom insertion
  syd::DicomSerieBuilder builder(db);
  builder.SetPatient(patient);
  builder.SetForcePatientFlag(args_info.forcePatient_flag);
  int i=0;
  for(auto f:files) {
    syd::loadbar(i, files.size());
    builder.SearchDicomInFile(f);
    ++i;
  }

  builder.InsertDicomSeries();
  // This is the end, my friend.
}
// --------------------------------------------------------------------
