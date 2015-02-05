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
#include "sydCheckIntegrity_ggo.h"
#include "core/sydCommon.h"
//#include "sydCheckIntegrityCommand.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydCheckIntegrity, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 1) {
    LOG(FATAL) << "Error number of arguments, please see usage";
  }

  // Get database
  std::string dbname = args_info.inputs[0];
  std::shared_ptr<syd::Database> dbg = syd::Database::OpenDatabase(dbname);

   // Generic CheckIntegrity
  std::vector<std::string> args;
  for(auto i=1; i<args_info.inputs_num; i++) args.push_back(args_info.inputs[i]);
  dbg->CheckIntegrity(args);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
