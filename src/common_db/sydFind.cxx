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
#include "sydTagHelper.h"
#include "sydPrintTable.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydFind, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();

  // Get the database
  syd::Database * db = syd::DatabaseManager::GetInstance()->Open(args_info.db_arg);

  // Simple dump if not arg
  if (args_info.inputs_num == 0) {
    db->Dump(std::cout);
    return EXIT_SUCCESS;
  }

  // Get the table name
  std::string table_name = args_info.inputs[0];

  // Dump list of fields for this table
  if (args_info.list_fields_flag) {
    std::ostringstream oss;
    auto map = db->GetTraits(table_name)->GetFieldsMap(db);
    oss << "Available fields for table " << table_name << ": " << std::endl;
    for(auto m:map) {
      if (m.first == m.second->name) oss << m.first << " ";
      else oss << m.second->name << "[" << m.first << "] ";
    }
    oss << std::endl << "Total of " << map.size() << " fields." << std::endl;
    auto fmap = db->GetTraits(table_name)->GetFieldFormatsMap(db);
    if (fmap.size() > 0) {
      oss << " Available formats : " << std::endl;
      for(auto m:fmap) oss << " - '" << m.first << "': " << m.second << std::endl;
    }
    LOG(0) << oss.str();
    return EXIT_SUCCESS;
  }

  // Prepare the list of arguments
  std::vector<std::string> patterns;
  for(auto i=1; i<args_info.inputs_num; i++)
    patterns.push_back(args_info.inputs[i]);

  // Prepare the list of arguments
  std::vector<std::string> exclude;
  for(auto i=0; i<args_info.exclude_given; i++)
    exclude.push_back(args_info.exclude_arg[i]);

  // Find all records
  syd::Record::vector records;
  if (!args_info.id_given)
    db->Query(records, table_name); // query all
  else {
    std::vector<syd::IdType> ids;
    for(auto i=0; i<args_info.id_given; i++) ids.push_back(args_info.id_arg[i]);
    db->Query(records, table_name, ids);
  }

  // Only keep the ones with the given tags (we do not check that the tags exist)
  if (args_info.tag_given and records.size() >0) {
    std::vector<std::string> tag_names;
    for(auto i=0; i<args_info.tag_given; i++)
      syd::GetWords(tag_names, args_info.tag_arg[i]);
    records = syd::GetRecordsThatContainAllTags<syd::Record>(records, tag_names);
  }

  // Grep
  syd::Record::vector results;
  if (args_info.find_all_fields_flag) {
    db->GrepAllFields(results, records, patterns, exclude);
    args_info.field_arg = "all";
  } else
    db->Grep(results, records, patterns, exclude);

  // Sort
  db->Sort(results, table_name, args_info.sort_arg);

  // Consider vv flag
  std::string format = args_info.field_arg;
  std::streambuf * buf = std::cout.rdbuf();
  std::ostringstream oss;
  if (args_info.vv_flag or args_info.vvs_flag) {
    if (args_info.vv_flag) oss << "vv ";
    if (args_info.vvs_flag) oss << "vv --sequence ";
    format = "filelist";
    buf = oss.rdbuf();
  }
  std::ostream os(buf);

  // Be sure to have exactly one result
  if (args_info.oneOutput_flag && results.size() == 0) {
    LOG(FATAL) << "Zero element found";
  }
  else if (args_info.oneOutput_flag && results.size() > 1) {
    LOG(FATAL) << "Several elements found";
  }
  if (results.size() == 0) {
    LOG(1) << "No records match";
    return EXIT_SUCCESS;
  }

  // Dump results
  if (args_info.list_flag) {
    // Get ids
    std::vector<syd::IdType> ids;
    for(auto r:results) ids.push_back(r->id);
    if (ids.size() == 0) return EXIT_SUCCESS;
    for(auto id:ids) std::cout << id << (id != ids.back() ? " ":"");
    std::cout << std::endl;
  }
  else {
    syd::PrintTable table;
    table.SetPrecision(args_info.precision_arg);
    table.Build(table_name, results, args_info.field_arg);
    table.SetHeaderFlag(!args_info.noheader_flag);
    table.SetFooterFlag(!args_info.nofooter_flag);
    table.SetSingleLineFlag(args_info.single_line_flag);
    table.Print(std::cout); // Print total number at the end !
    LOG(1) << results.size() << " elements found in table " << table_name;
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

  // VV
  if (args_info.vv_flag or args_info.vvs_flag) {
    syd::PrintTable table;
    table.Build(table_name, results, "filepath");
    table.SetHeaderFlag(false);
    table.SetFooterFlag(false);
    table.SetSingleLineFlag(true);
    table.Print(oss);
    table.Print(std::cout);
    LOG(1) << "Executing the following command: " << std::endl << oss.str();
    int r = syd::ExecuteCommandLine(oss.str(), 2);
  }

  // Delete
  if (args_info.delete_flag) {
    if (results.size() > 0) {
      if (!args_info.force_flag) {
        std::cout << "Really delete " << results.size() << " element"
                  << (results.size() > 1 ? "s ":" ") << "(y/n) ? ";
        char c;
        std::scanf("%c", &c);
        if (c =='y') {
          db->Delete(results, table_name);
          LOG(1) << results.size() << " elements deleted.";
        }
        else { LOG(FATAL) << "Abort."; }
      }
      else {
        db->Delete(results, table_name);
        LOG(1) << results.size() << " elements deleted.";
      }
    }
  }

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
