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
#include "core/sydCommon.h"
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydDump, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 1) {
    //LOG(FATAL) << "Error please, provide <db> <cmd|patient> (see usage)";

    //    Display info for all dbs
    syd::DatabaseFactory::OpenDatabaseFilenames();
    for(auto i:syd::DatabaseFactory::map_of_database_types_) {
      std::string param = syd::DatabaseFactory::map_of_database_param_[i.first];
      std::istringstream f(param);
      std::string filename;
      getline(f, filename, ';');
      std::string folder;
      getline(f, folder, ';');
      std::cout << "Database '" << i.first << "' of type " << i.second << std::endl
                << "\t filename " << filename << " in folder " << folder << std::endl;
    }
    exit(0); // end
  }

  // Get database
  std::string dbname = args_info.inputs[0];
  std::shared_ptr<syd::Database> dbg = syd::Database::OpenDatabase(dbname);

  // Generic Dump
  std::vector<std::string> args;
  for(auto i=1; i<args_info.inputs_num; i++) args.push_back(args_info.inputs[i]);
  dbg->Dump(std::cout, args);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
