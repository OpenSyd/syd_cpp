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
#include "sydCommonGengetopt.h"

#include <cstdio>

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydDelete, 1); // 1 because can read ids from the pipe

  // Load plugin
  syd::PluginManager::GetInstance()->Load();

  // Load the database
  syd::Database * db = syd::DatabaseManager::GetInstance()->Read(args_info.db_arg);

  // Get the table name
  std::string tablename = args_info.inputs[0];

  // Set option
  // FIXME db->SetDeleteForceFlag(args_info.force_flag);

  // Get the list of ids
  int n=0;
  if (args_info.inputs_num > 1 and args_info.inputs[1] == std::string("all")) {
    syd::Record::vector v;
    db->Query(v, tablename);
    if (v.size() == 0) return 1;
    std::cout << "Really delete " << v.size() << " element" << (v.size() > 1 ? "s ":" ") << "(y/n) ? ";
    char c;
    std::scanf("%c", &c);
    if (c =='y') db->Delete(v, tablename);
    else {
      LOG(FATAL) << "Abort.";
    }
    n = v.size();
  }
  else {
    std::vector<syd::IdType> ids;
    syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
    for(auto i=1; i<args_info.inputs_num; i++) {
      ids.push_back(atoi(args_info.inputs[i]));
    }
    try {
      syd::Record::vector v;
      db->Query(v, tablename, ids);
      if (v.size() == 0) return 1;
      std::cout << "Really delete " << v.size() << " element" << (v.size() > 1 ? "s ":" ") << "(y/n) ? ";
      char c;
      std::scanf("%c", &c);
      if (c =='y') {
        db->Delete(v, tablename);
      }
      else {
        LOG(FATAL) << "Abort.";
      }
      n = v.size();
    }
    catch (std::exception & e) {
      LOG(FATAL) << "Cannot delete " << ids.size() << " elements of table "
                 << tablename << std::endl
                 << e.what();
    }
  }
  if (n== 0) { LOG(1) << "No elements has been deleted"; }
  if (n== 1) { LOG(1) << "One element has been deleted"; }
  if (n>1)   { LOG(1) << n << " elements has been deleted."; }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
