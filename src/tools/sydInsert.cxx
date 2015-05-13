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
#include "sydInsert_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(sydInsert, 2);

  // Get params
  std::string dbname = args_info.inputs[0];
  std::string tablename = args_info.inputs[1];
  std::vector<std::string> args;
  for(auto i=2; i<args_info.inputs_num; i++)
    args.push_back(args_info.inputs[i]);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();

    // Load the database
  syd::Database * db = syd::DatabaseManager::GetInstance()->Read(dbname);

  // Insert
  syd::TableElement * e = db->InsertFromArg(tablename, args);
  LOG(1) << "Insertion done: " << e->ToString();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
