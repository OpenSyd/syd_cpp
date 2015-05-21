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
#include "sydDump_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(sydDump, 1);

  // Get db name
  std::string dbname = args_info.inputs[0];

  // Load plugin
  syd::PluginManager::GetInstance()->Load();

  // Load the database
  syd::Database * db;
  try {
    db = syd::DatabaseManager::GetInstance()->Read(dbname);
  } catch (syd::Exception & e) {
    LOG(FATAL) << e.what();
  }

  // Prepare the list of arguments
  std::vector<std::string> args;
  for(auto i=1; i<args_info.inputs_num; i++)
    args.push_back(args_info.inputs[i]);

  // Dump
  db->Dump(args, std::cout);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
