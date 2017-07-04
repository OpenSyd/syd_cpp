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
#include "sydDicomBuilder.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydCommentsHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertDicom, 1);

  // Load plugin and get the database
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of folders to look for
  std::vector<std::string> files_folders;
  for(auto i=0; i<args_info.inputs_num; i++) files_folders.push_back(args_info.inputs[i]);

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

  // Get the patient if needed
  syd::Patient::pointer patient = nullptr;
  if (args_info.patient_given)
    patient = db->FindPatient(args_info.patient_arg);

  // Dicom insertion
  syd::DicomBuilder builder(db, args_info.updatePatient_flag);
  builder.SetForceDicomFileUpdateFlag(args_info.force_flag);
  int i=0;
  for(auto f:files) {
    syd::loadbar(i, files.size());
    builder.SearchDicomInFile(f, patient);
    ++i;
  }
  // Insert dicom and update comments if needed
  auto series = builder.GetDicomSeries();
  for(auto d:series) syd::SetCommentsFromCommandLine(d->comments, db, args_info);
  auto structs = builder.GetDicomStruct();
  for(auto d:structs) syd::SetCommentsFromCommandLine(d->comments, db, args_info);
  builder.InsertDicom();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
