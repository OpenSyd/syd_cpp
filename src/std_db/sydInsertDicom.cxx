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
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the patient
  std::string name = args_info.inputs[0];
  auto patient = db->FindPatient(name);

  // Get the injection
  std::string inj = args_info.inputs[1];
  syd::Injection::pointer injection;
  if (syd::IsInteger(inj)) { // this is an injection id
    try {
      db->QueryOne(injection, atoi(inj.c_str())); // exception if not found
      if (injection->patient->id != patient->id) EXCEPTION(""); // exception if not the same patient
    } catch(std::exception & e) {
      LOG(FATAL) << "Cannot find injection id '" << inj << "' for this patient.";
    }
  }
  else { // try to find injections with same radionuclide
    syd::Injection::vector injections;
    odb::query<syd::Injection> q =
      odb::query<syd::Injection>::patient == patient->id and
      odb::query<syd::Injection>::radionuclide->name == inj;
    db->Query(injections, q);
    if (injections.size() == 0) {
      LOG(FATAL) << "Cannot find injection with radionuclide '" << inj << "' for this patient.";
    }
    if (injections.size() > 1) {
      std::string s;
      for(auto i:injections) s += "\t"+i->ToString()+'\n';
      s.pop_back();// remove last endline
      LOG(FATAL) << "I found several injections of the radionuclide '" << inj
                 << "', please choose only one (give the id):" << std::endl << s;
    }
    injection = injections[0];
  }
  LOG(1) << "Associated injection: " << injection;

  // Get the list of folders to look for
  std::vector<std::string> folders;
  for(auto i=2; i<args_info.inputs_num; i++) folders.push_back(args_info.inputs[i]);

  // Dicom insertion
  syd::DicomSerieBuilder b(db);
  b.SetInjection(injection); // FIXME change to SetPatient if no injection ?
  b.SetForcePatientFlag(args_info.forcePatient_flag);
  //  b.SetForceUpdateFlag(args_info.forceUpdate_flag); //FIXME ?
  for(auto folder:folders) {
    LOG(2) << "Search for files in " << folder;
    if (!fs::exists(folder.c_str())) {
      LOG(WARNING) << "The directory " << folder << " does not exist.";
      continue;
    }
    std::vector<std::string> files;
    syd::SearchForFilesInFolder(files, folder, true);
    int n = files.size();
    int i=0;
    LOG(1) << "Searching for dicom series in " << files.size() << " files ...";
    for(auto f:files) {
      syd::loadbar(i,n);
      b.CreateDicomSerieFromFile(f);
      ++i;
    }
  }
  LOG(1) << "Copying files to db ...";
  b.InsertDicomSeries();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
