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
#include "sydCropCT_ggo.h"
#include "core/sydCommon.h"
#include "sydCropCTCommand.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydCropCT, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 2) {
    LOG(FATAL) << "Error please, provide <db> <patient> <a> (see usage)";
  }

  // Get the dbs
  std::string db  = args_info.inputs[0];
  syd::CropCTCommand * c = new syd::CropCTCommand(db);

  // Go
  std::string patient_name = args_info.inputs[1];
  std::vector<std::string> arg;
  for(auto i=2; i<args_info.inputs_num; i++) arg.push_back(args_info.inputs[i]);
  c->set_ignore_md5_flag(args_info.ignore_flag);
  c->Run(patient_name, arg);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
