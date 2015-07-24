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

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertDicom, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the patient
  std::string name = args_info.inputs[0];
  auto patient = db->FindPatient(name);

  // Get the injection
  std::string inj = args_info.inputs[1];
  auto injection = db->FindInjection(patient, inj);
  // FIXME 0 if no injection

  // Get the list of folders to look for
  std::vector<std::string> folders;
  for(auto i=2; i<args_info.inputs_num; i++) folders.push_back(args_info.inputs[i]);

  // Dicom insertion
  syd::DicomSerieBuilder b(db);
  b.SetInjection(injection); // FIXME change to SetPatient if no injection
  b.SetForcePatientFlag(args_info.forcePatient_flag);
  //  b.SetForceUpdateFlag(args_info.forceUpdate_flag); //FIXME
  for(auto f:folders) {
    OFList<OFString> files;
    b.SearchForFilesInFolder(f, files);
    int n = files.size();
    int i=0;
    LOG(1) << "Searching for dicom series in " << files.size() << " files ...";
    for(auto f:files) {
      syd::loadbar(i,n);
      b.CreateDicomSerieFromFile(f.c_str());
      ++i;
    }
  }
  LOG(1) << "Copying files to db ...";
  b.InsertDicomSeries();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
