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
#include "sydDumpTimepoint_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydStandardDatabase.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(sydDumpTimepoint, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Prepare the list of arguments
  std::vector<std::string> args;
  for(auto i=1; i<args_info.inputs_num; i++)
    args.push_back(args_info.inputs[i]);

  // FIXME : only select timepoint of a given tag + patient

  // Dump
  //  db->DumpTimepoint(args, std::cout, args_info.verboseResult_flag);
  std::vector<syd::Timepoint> timepoints;
  db->Query(timepoints);

  for(auto t:timepoints) {
    DD(t);
    for(auto d:t.dicoms) {
      std::cout << "\t " << *d << std::endl;
    }
  }


  // This is the end, my friend.
}
// --------------------------------------------------------------------
