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
#include "sydActivity_ggo.h"
#include "core/sydCommon.h"
#include "sydActivityCommand.h"

// easylogging : only once initialization (in the main)
_INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydActivity, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 2) {
    LOG(FATAL) << "Error please, provide <db> TODO";
  }

  // Get the current db names
  std::string db = args_info.inputs[0];
  syd::ActivityCommand * c = new syd::ActivityCommand(db);

  // Execute the command
  std::vector<std::string> args;
  for(auto i=1; i<args_info.inputs_num; i++) args.push_back(args_info.inputs[i]);
  c->Run(args);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
