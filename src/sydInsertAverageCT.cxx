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
#include "sydInsertAverageCT_ggo.h"
#include "core/sydCommon.h"
#include "sydInsertAverageCTCommand.h"

// easylogging : only once initialization (in the main)
_INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydInsertAverageCT, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 2) {
    LOG(FATAL) << "Error please, provide <db> <patient> (see usage)";
  }

  // Get the dbs
  std::string db = args_info.inputs[0];
  syd::InsertAverageCTCommand * c = new syd::InsertAverageCTCommand(db);

  // Go
  std::string patient_name = args_info.inputs[1];
  std::vector<std::string> fake;
  c->Run(patient_name, fake);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
