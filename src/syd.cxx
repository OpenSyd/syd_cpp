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
#include "sydQuery.h"
#include "syd_ggo.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(syd, args_info);

  // Start opening the db
  syd::sydQuery syd;
  syd.SetVerboseFlag(args_info.verbose_flag);
  syd.SetVerboseDBFlag(args_info.verboseDB_flag);
  syd.SetVerboseQueryFlag(args_info.verboseQuery_flag);
  syd.OpenDatabase();

  if (args_info.updateRoiSerie_given) {
    syd.UpdateActivity(args_info.inputs[0], args_info.inputs[1]);
  }

  if (args_info.updateSPECT_given) {
    DD("TODO");
    DD("ici spect");
  }

}
// --------------------------------------------------------------------
