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
#include "sydCheckIntegrityCommand.h"

// easylogging : only once initialization (in the main)
_INITIALIZE_EASYLOGGINGPP

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydCheckIntegrity, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Get the current db names/folders
  syd::CheckIntegrityCommand * c = new syd::CheckIntegrityCommand;
  c->OpenDatabases();
  c->set_check_file_content_level(args_info.level_arg);
  c->SetArgs(args_info.inputs, args_info.inputs_num);
  c->Run();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
