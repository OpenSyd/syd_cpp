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

  // Consider folder to search in. Convert to absolute path to be
  // compared to absolute path in the db.
  std::string folder= db->GetDatabaseAbsoluteFolder();
  if (args_info.folder_given) folder = args_info.folder_arg;
  syd::ConvertToAbsolutePath(folder);

  // List of files folder
  std::vector<std::string> complete_filenames;
  syd::SearchForFilesInFolder(complete_filenames, folder, args_info.recurse_flag);
  std::sort(complete_filenames.begin(), complete_filenames.end());

  // Extract the filename (without path)
  std::vector<std::string> filenames;
  for(auto f:complete_filenames) {
    filenames.push_back(syd::GetFilenameFromPath(f));
  }
  LOG(1) << "Found " << filenames.size() << " files in the folder.";

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

  std::vector<std::string> missing_files;
  int n = complete_filenames.size();
  int i=0;
  for(auto f:complete_filenames) {
    auto r = std::find_if(files.begin(), files.end(),
                          [f, db](syd::File::pointer & file) {
                            return (f == file->GetAbsolutePath(db)); });
    if (r == files.end()) {
      missing_files.push_back(f);
    }
    else {
      LOG(2) << *r << " is in the db.   ";
      files.erase(r);
    }
    if (!args_info.no_loadbar_flag)  syd::loadbar(i,n);
    ++i;
  }

  // Print the results, and delete if needed
  if (missing_files.size() == 0) {
    LOG(1) << "All the " << complete_filenames.size() << " files are in the db.";
  }
  else {
    LOG(0) << "There are " << missing_files.size() << " files that are *not* in the db. ";
    for(auto f:missing_files) LOG(1) << f;
  }

  if (args_info.delete_flag and missing_files.size() > 0) {
    std::string m="";
    if (sydlog::Log::LogLevel() == 0) m = " (use -v1 to see the file)";
    std::cout << redColor
              << "Really delete " << missing_files.size()
              << " files (y/n)" << m << " ? " << resetColor;
    char c;
    std::cin >> c;
    if (c =='y') {
      for(auto f:missing_files) {
        LOG(1) << "Deleting " << f << " ...";
        fs::remove(f);
      }
      LOG(0) << "Files deleted.";
    }
    else LOG(0) << "Abort.";
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
