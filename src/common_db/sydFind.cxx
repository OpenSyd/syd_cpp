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
#include "sydCommonGengetopt.h"

// Init syd
SYD_STATIC_INIT


// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydFind, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();

  // Get the database
  syd::Database * db = syd::DatabaseManager::GetInstance()->Read(args_info.db_arg);

  // Simple dump if not arg
  if (args_info.inputs_num == 0) {
    db->Dump(std::cout);
    return EXIT_SUCCESS;
  }

  // Get the table name
  std::string table_name = args_info.inputs[0];

  // Prepare the list of arguments
  std::vector<std::string> patterns;
  for(auto i=1; i<args_info.inputs_num; i++)
    patterns.push_back(args_info.inputs[i]);

  // Prepare the list of arguments
  std::vector<std::string> exclude;
  for(auto i=0; i<args_info.exclude_given; i++)
    exclude.push_back(args_info.exclude_arg[i]);

  // Find
  syd::Record::vector records;
  db->Query(records, table_name);

  // Grep
  syd::Record::vector results;
  db->Grep(results, records, patterns, exclude);

  // Sort
  db->Sort(results, table_name);

  // Dump results
  if (args_info.list_flag) {
    // Get ids
    std::vector<syd::IdType> ids;
    for(auto r:results) ids.push_back(r->id);
    if (ids.size() == 0) return EXIT_SUCCESS;
    for(auto id:ids) std::cout << id << " ";
    std::cout << std::endl;
  }
  else {
    syd::PrintTable table;
    table.SetFormat(args_info.format_arg);
    table.SetHeaderFlag(!args_info.noheader_flag);
    table.Dump<syd::Record>(results);
  }

  // Check
  if (args_info.check_flag) {
    LOG(1) << "Checking ...";
    std::vector<syd::IdType> ids_error;
    int i=0;
    int n = results.size();
    for(auto r:results) {
      auto res = r->Check();
      if (!res.success) {
        LOG(WARNING) << "Error for " << r << ": " << res.description;
        ids_error.push_back(r->id);
      }
      syd::loadbar(i,n);
      ++i;
    }
    for(auto i:ids_error) std::cout << i << " ";
    if (ids_error.size() != 0) std::cout << std::endl;
    LOG(1) << "Number of error: " << ids_error.size() << "                            ";
  }

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
