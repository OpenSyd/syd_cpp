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
#include "sydInsertTimepoint_ggo.h"
#include "core/sydCommon.h"
#include "sydInsertTimepointCommand.h"

// easylogging : only once initialization (in the main)
_INITIALIZE_EASYLOGGINGPP

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydInsertTimepoint, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 3) {
    LOG(FATAL) << "Error please, provide <db1> <db2> <serie ids> (see usage)";
  }

  // Get the current db names
  std::string db1 = args_info.inputs[0];
  std::string db2 = args_info.inputs[1];
  syd::InsertTimepointCommand * c = new syd::InsertTimepointCommand(db1, db2);

  // Execute the command
  c->set_ct_selection_patterns(args_info.ct_arg);
  std::vector<std::string> serie_ids;
  for(auto i=2; i<args_info.inputs_num; i++) serie_ids.push_back(args_info.inputs[i]);
  c->InsertTimepoint(serie_ids);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
