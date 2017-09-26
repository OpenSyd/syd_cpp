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
#include "sydOrganAbsorbedDose_ggo.h"
#include "sydCommonGengetopt.h"
#include "sydSCoefficientCalculator.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydOrganAbsorbedDose, 3);

  // Parameters
  auto source_name = args_info.inputs[0];
  auto target_name = args_info.inputs[1];
  auto rad_name = args_info.inputs[2];
  auto phantom_name = "AM"; // Adult Male FIXME --> will be in ggo
  auto folder = args_info.folder_arg;

  // Compute the S coefficient
  syd::SCoefficientCalculator * c = new syd::SCoefficientCalculator;
  c->Initialise(folder);
  c->SetSourceOrgan(source_name);
  c->SetTargetOrgan(target_name);
  c->SetRadionuclide(rad_name);
  c->SetPhantomName(phantom_name);
  auto s = c->Run();

  LOG(0) << s << " mGy/MBq.h";

  // -----------------------------------------------------------------
  DD("end");
  // This is the end, my friend.
}
// --------------------------------------------------------------------
