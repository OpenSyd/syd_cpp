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
#include "sydComputeICRPSCoefficient_ggo.h"
#include "sydCommonGengetopt.h"
#include "sydSCoefficientCalculator.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydComputeICRPSCoefficient, 3);

  // Parameters
  auto source_name = args_info.inputs[0];
  auto target_name = args_info.inputs[1];
  auto rad_name = args_info.inputs[2];
  auto phantom_name = args_info.phantom_arg;
  auto folder = args_info.folder_arg;

  // Initialise the calculator
  syd::SCoefficientCalculator * c = new syd::SCoefficientCalculator;
  c->Initialise(folder);
  c->SetSourceOrgan(source_name);
  c->SetTargetOrgan(target_name);
  c->SetRadionuclide(rad_name);
  c->SetPhantomName(phantom_name);

  // print if needed
  if (args_info.printOrgans_flag) {
    auto list = c->GetListOfSourceOrgans();
    std::cout << "Source: ";
    for(auto l:list)
      std::cout << l << " ";
    std::cout << std::endl;
    list= c->GetListOfTargetOrgans();
    std::cout << "Target: ";
    for(auto l:list)
      std::cout << l << " ";
    std::cout << std::endl;
  }

  // Compute the S coefficient
  auto s = c->Run();

  // -----------------------------------------------------------------
  std::cout << source_name << " "
            << target_name << " "
            << rad_name << " "
            << s << " mGy/MBq.h" << std::endl;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
