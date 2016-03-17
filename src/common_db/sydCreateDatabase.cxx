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
#include "sydCommonGengetopt.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // init ggo
  SYD_INIT_GGO(sydCreateDatabase, 0); // allow zero param for option -l

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager * m = syd::DatabaseManager::GetInstance();

  // Display help (-l)
  if (args_info.list_flag) {
    // Get the list of database if needed.
    auto & list = m->GetDatabaseSchemas();
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

  if (fs::exists(dbname) and !args_info.overwrite_flag) {
    LOG(FATAL) << "Database not created: the file '" << dbname << "' already exists.";
  }
  // It is *required* to delete before. If not inconsistency could happens.
  if (fs::exists(dbname)) {
    LOG(WARNING) << "Deleting the file '" << dbname << "' (.backup).";
    std::string b = dbname+".backup";
    int result = std::rename(dbname.c_str(), b.c_str());
    if (result != 0) {
      LOG(FATAL) << "Error while renaming " << dbname << " to " << b;
    }
  }

  // Create the database
  m->Create(dbtype, dbname, folder);
  syd::Database * db = m->Open(dbname);
  LOG(1) << "Database " << dbname << " of type '" << dbtype << "' created.";
  LOG(2) << "List of tables: " << db->GetListOfTableNames();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
