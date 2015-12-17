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
#include "sydCheck_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydFile.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydCheck, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();

  // Load the database
  syd::Database * db;
  db = syd::DatabaseManager::GetInstance()->Read(args_info.db_arg);

  //  db->Dump(std::cout);

  std::string folder= db->GetDatabaseAbsoluteFolder();
  if (args_info.folder_given) folder = args_info.folder_arg;
  DD(folder);

  // list of files folder
  std::vector<std::string> complete_filenames;
  syd::SearchForFilesInFolder(complete_filenames, folder, "*", true);
  std::sort(complete_filenames.begin(), complete_filenames.end());
  //DDS(files);

  // extract the filename (without path)
  std::vector<std::string> filenames;
  for(auto f:complete_filenames) {
    filenames.push_back(syd::GetFilenameFromPath(f));
  }
  DDS(filenames);
  DDS(complete_filenames);

  // For each look in files for folder, then files
  syd::File::vector files;
  typedef odb::query<syd::File> Q;
  Q q = Q::filename.in_range(filenames.begin(), filenames.end());
  db->Query(files, q);
  std::sort(files.begin(), files.end(),
            [db](const syd::File::pointer & a, const syd::File::pointer & b) {
              std::string aa = a->GetAbsolutePath(db);
              std::string bb = a->GetAbsolutePath(db);
              return aa > bb;
            });
  DDS(files);
  DD(files.size());
  DD(files[0]->GetAbsolutePath(db));
  DD(complete_filenames[0]);
  for(auto f:complete_filenames) {
    DD(f);
    auto r = std::find_if(files.begin(), files.end(),
                          [f, db](syd::File::pointer & file) {
                            DD(f);
                            DD(file->GetAbsolutePath(db));
                            bool b = (f == file->GetAbsolutePath(db));
                            DD(b);
                            return b;
                          });
    if (r == files.end()) {
      DD("not found");
    }
    else {
      DD("found");
    }
  }


  // This is the end, my friend.
}
// --------------------------------------------------------------------
