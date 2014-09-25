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
#include "sydDumpStudy_ggo.h"
#include "core/sydCommon.h"
#include "sydDumpStudyCommand.h"

// easylogging : only once initialization (in the main)
_INITIALIZE_EASYLOGGINGPP

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydDumpStudy, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 3) {
    LOG(FATAL) << "Error please, provide <db1> <db2> <patient> (see usage)";
  }

  // Get the current db names
  std::string db1 = args_info.inputs[0];
  std::string db2 = args_info.inputs[1];
  syd::DumpStudyCommand * c = new syd::DumpStudyCommand(db1, db2);

  // Execute the command
  std::string patient = args_info.inputs[2];
  c->Dump(patient);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
