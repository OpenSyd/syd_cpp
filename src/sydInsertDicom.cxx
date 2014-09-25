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
#include "sydInsertDicom_ggo.h"
#include "core/sydCommon.h"
#include "sydInsertDicomCommand.h"

// easylogging : only once initialization (in the main)
_INITIALIZE_EASYLOGGINGPP

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydInsertDicom, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 3) {
    LOG(FATAL) << "Error please, provide <db> <patient_name> <folders> (see usage)";
  }

  // Get the current db names
  std::string db = args_info.inputs[0];
  syd::InsertDicomCommand * c = new syd::InsertDicomCommand(db);

  // Get the current db names/folders
  c->set_rename_flag(args_info.rename_flag);
  std::string patient_name = args_info.inputs[1];
  std::vector<std::string> folders;
  for(auto i=2; i<args_info.inputs_num; i++) folders.push_back(args_info.inputs[i]);
  c->InsertDicom(patient_name, folders);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
