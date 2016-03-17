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
  syd::Database * db = syd::DatabaseManager::GetInstance()->Open(args_info.db_arg);

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

  // Find all records
  syd::Record::vector records;
  if (args_info.tag_given) {
    std::vector<std::string> tag_names;
    for(auto i=0; i<args_info.tag_given; i++)
      syd::GetWords(tag_names, args_info.tag_arg[i]);
    db->QueryByTag(records, table_name, tag_names);
  }
  else { // Query all record
    db->Query(records, table_name);
  }

  // Grep
  syd::Record::vector results;
  db->Grep(results, records, patterns, exclude);

  // Sort
  db->Sort(results, table_name);

  // Consider vv flag
  std::string format = args_info.format_arg;
  std::streambuf * buf = std::cout.rdbuf();
  std::ostringstream oss;
  if (args_info.vv_flag or args_info.vvs_flag) {
    if (args_info.vv_flag) oss << "vv ";
    if (args_info.vvs_flag) oss << "vv --sequence ";
    format = "filelist";
    buf = oss.rdbuf();
  }
  std::ostream os(buf);

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
    if (results.size() == 0) {
      LOG(1) << "No records match";
      return EXIT_SUCCESS;
    }
    syd::PrintTable table;
    table.SetFormat(format);
    table.SetHeaderFlag(!args_info.noheader_flag);
    results[0]->InitTable(table);
    for(auto i=0; i<args_info.col_given; i++) {
      std::string s = args_info.col_arg[i];
      std::vector<std::string> w;
      syd::GetWords(w, s);
      if (w.size() != 3) {
        LOG(FATAL) << "Format must be 3 strings: 'num_col' 'p' 'value'";
      }
      int col = atoi(w[0].c_str());
      if (w[1] != "p") {
        LOG(FATAL) << "Format not known. Must be 'p'.";
      }
      int v = atoi(w[2].c_str());
      table.SetColumnPrecision(col, v);
    }

    try {
      table.Dump<syd::Record>(results, os);
    } catch (std::exception & e) {
      if (args_info.vv_flag or args_info.vvs_flag) {
        LOG(FATAL) << "Error, results *must* be images with filenames to be able to be open with vv"
                   << std::endl << "Query error is: " << e.what();
      }
    }
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
    LOG(1) << "Executing the following command: " << std::endl << oss.str();
    int r = syd::ExecuteCommandLine(oss.str(), 2);
    // Stop if error in cmd
    if (r == -1) {
      LOG(WARNING) << "Error while executing the following command: " << std::endl << oss.str();
    }
  }

  // Delete
  if (args_info.delete_flag) {
    if (results.size() > 0) {
      std::cout << "Really delete " << results.size() << " element"
                << (results.size() > 1 ? "s ":" ") << "(y/n) ? ";
      char c;
      std::scanf("%c", &c);
      if (c =='y') db->Delete(results, table_name);
      else { LOG(FATAL) << "Abort."; }
    }
  }

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
