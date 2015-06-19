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
#include "sydFind_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(sydFind, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::Database * db = syd::DatabaseManager::GetInstance()->Read(dbname);

  // Get the table name
  std::string table_name = args_info.inputs[1];

  // Prepare the list of arguments
  std::vector<std::string> patterns;
  for(auto i=2; i<args_info.inputs_num; i++)
    patterns.push_back(args_info.inputs[i]);

  // Prepare the list of arguments
  std::vector<std::string> exclude;
  for(auto i=0; i<args_info.exclude_given; i++)
    exclude.push_back(args_info.exclude_arg[i]);

  // Find
  std::vector<syd::IdType> ids;
  db->Find(ids, table_name, patterns, exclude);
  if (ids.size() == 0) return EXIT_SUCCESS;

  // Dump results
  if (!args_info.dump_flag) {
    for(auto id:ids) std::cout << id << " ";
    std::cout << std::endl;
  }
  else {
    db->Dump(std::cout, table_name, args_info.format_arg, ids);
  }

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
