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
#include "sydRegister_ggo.h"
#include "core/sydCommon.h"
#include "sydRegisterCommand.h"

// easylogging : only once initialization (in the main)
_INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydRegister, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 5) {
    LOG(FATAL) << "Error please, provide <db1> <db2> <patient> <a> <b> (see usage)";
  }

  // Get the dbs
  std::string db1 = args_info.inputs[0];
  std::string db2 = args_info.inputs[1];
  syd::RegisterCommand * c = new syd::RegisterCommand(db1, db2);

  // Go
  std::string patient_name = args_info.inputs[2];
  c->set_config_filename(args_info.elconfig_arg);
  int a = atoi(args_info.inputs[3]);
  int b = atoi(args_info.inputs[4]);
  c->Run(patient_name, a, b);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
