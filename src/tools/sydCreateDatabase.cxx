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
#include "sydCreateDatabase_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // init ggo
  SYD_INIT(sydCreateDatabase, 0); // allow zero param for option -l

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager * m = syd::DatabaseManager::GetInstance();
  auto & list = m->GetDatabaseSchemas();

  if (args_info.list_flag) {
    if (list.size() == 0) {
      LOG(FATAL) << "No database type registered. No plugin found ? Try to check the SYD_PLUGIN environement variable.";
    }
    if (list.size() > 1) std::cout << "There are " << list.size() << " registered database types: ";
    else std::cout  << "There is a single registered database type: ";
    for(auto i=list.begin(); i != list.end(); i++) {
      std::cout << *i << " ";
    }
    std::cout << std::endl;
    exit(0);
  }

  // Check param
  if (args_info.inputs_num < 3) {
    cmdline_parser_sydCreateDatabase_print_help();
    LOG(FATAL) << "Please provide at least 3 param";
  }
  std::string dbtype = args_info.inputs[0];
  std::string dbname = args_info.inputs[1];
  std::string folder = args_info.inputs[2];

  if (syd::FileExists(dbname) and !args_info.overwrite_flag) {
    LOG(FATAL) << "Database not created: the file '" << dbname << "' already exists.";
  }
  // It is *required* to delete before. If not inconsistency could happens.
  if (syd::FileExists(dbname)) {
    LOG(WARNING) << "Deleting the file '" << dbname << "' (.backup).";
    std::string b = dbname+".backup";
    int result = std::rename(dbname.c_str(), b.c_str());
    if (result != 0) {
      LOG(FATAL) << "Error while renaming " << dbname << " to " << b;
    }
  }

  // Set the folder in the db_info table
  if (!syd::DirExists(folder)) {
    LOG(WARNING) << "The folder '" << folder << "' does not exist, I create it.";
    syd::CreateDirectory(folder);
  }
  std::string dbname_absolute = dbname;
  syd::ConvertToAbsolutePath(dbname_absolute);
  std::string folder_absolute = folder;
  syd::ConvertToAbsolutePath(folder_absolute);
  dbname_absolute = syd::GetPathFromFilename(dbname_absolute);
  folder_absolute = syd::GetPathFromFilename(folder_absolute);
  if (folder_absolute != dbname_absolute) {
    LOG(FATAL) << "The database file and the image folder must be in the same directory. Here '"
               << dbname << "' is in '" << dbname_absolute
               << "' while the folder '" << folder << "' is in '" << folder_absolute;
  }

  // Create the database
  syd::Database * db = m->Create(dbtype, dbname, folder);
  LOG(1) << "Database " << dbname << " of type '" << dbtype << "' created.";
  LOG(2) << "List of tables: " << db->GetListOfTableNames();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
