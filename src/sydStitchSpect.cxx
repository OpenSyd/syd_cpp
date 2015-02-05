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
#include "sydStitchSpect_ggo.h"
#include "core/sydCommon.h"
#include "sydStitchSpectCommand.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydStitchSpect, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 2) {
    LOG(FATAL) << "Error please, provide <db1> <serie ids> (see usage)";
  }

  // Get the current db names
  std::string db = args_info.inputs[0];
  syd::StitchSpectCommand * c = new syd::StitchSpectCommand(db);

  // Execute the command
  std::vector<std::string> serie_ids;
  c->set_threshold_cumul(args_info.thresholdCumul_arg);
  c->set_skip_slices(args_info.skip_arg);
  for(auto i=1; i<args_info.inputs_num; i++) serie_ids.push_back(args_info.inputs[i]);
  c->StitchSpect(serie_ids);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
