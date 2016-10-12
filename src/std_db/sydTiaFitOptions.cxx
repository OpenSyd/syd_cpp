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
#include "sydTiaFitOptions.h"

// --------------------------------------------------------------------
syd::TiaFitOptions::TiaFitOptions()
{
  r2_min = 0.9;
  max_iteration = 50;
  restricted_tac = false;
  akaike_criterion = "AIC";
  // FIXME additional points
  // FIXME post processes
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TiaFitOptions::GetModelsName() const
{
  std::stringstream ss;
  for(auto m:models_name) ss << m << " ";
  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


