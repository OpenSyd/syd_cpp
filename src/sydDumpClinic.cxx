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
#include "sydDumpClinic_ggo.h"
#include "core/sydCommon.h"
#include "sydDumpClinicCommand.h"

// easylogging : only once initialization (in the main)
_INITIALIZE_EASYLOGGINGPP

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydDumpClinic, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 2) {
    LOG(FATAL) << "Error please, provide <db> <cmd> <patient> [<patterns>] (see usage)";
  }

  // Get the current db name
  std::string db = args_info.inputs[0];
  syd::DumpClinicCommand * c = new syd::DumpClinicCommand(db);

  // Execute the command
  std::string cmd = args_info.inputs[1];
    std::vector<std::string> patterns;
    if (args_info.inputs_num > 2) {
      std::string patient = args_info.inputs[2];
      for(auto i=3; i<args_info.inputs_num; i++) patterns.push_back(args_info.inputs[i]);
      c->Dump(cmd, patient, patterns);
  }
  else c->Dump(cmd, "", patterns);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
