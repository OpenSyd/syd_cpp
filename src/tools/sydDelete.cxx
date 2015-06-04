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
#include "sydDelete_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(sydDelete, 3);

  // Get params
  std::string dbname = args_info.inputs[0];

  // Load plugin
  syd::PluginManager::GetInstance()->Load();

  // Load the database
  syd::Database * db = syd::DatabaseManager::GetInstance()->Read(dbname);

  // Get the table name
  std::string tablename = args_info.inputs[1];

  // Set option
  db->SetDeleteDryRunFlag(args_info.dry_run_flag);

  // Get the list of ids
  if (args_info.inputs[2] == std::string("all")) {
    db->DeleteAll(tablename);
  }
  else {
    std::vector<syd::IdType> ids;
    for(auto i=2; i<args_info.inputs_num; i++) {
      ids.push_back(atoi(args_info.inputs[i]));
    }
    db->Delete(tablename, ids);
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------