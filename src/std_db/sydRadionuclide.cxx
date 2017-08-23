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
#include "sydRadionuclide.h"
#include "sydStandardDatabase.h"
#include "sydRecordTraits.h"

DEFINE_TABLE_IMPL(Radionuclide);

// --------------------------------------------------------------------
syd::Radionuclide::Radionuclide():syd::Record()
{
  name = empty_value;
  metastable = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------
std::string syd::Radionuclide::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << name << " "
     << half_life_in_hours << " "
     << element << " "
     << atomic_number << " "
     << mass_number << " "
     << (metastable? "metastable ":empty_value) << " "
     << max_beta_minus_energy_in_kev;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Radionuclide::Set(const std::vector<std::string> & arg)
{
  if (arg.size() < 7) {
    LOG(FATAL) << "To insert radionuclide, please set <name> <half_life_in_hours> "
               << "<element> <atomic_number> <mass_number> <metasable> <max_beta_minus_energy_in_kev>]";
  }
  name = arg[0];
  half_life_in_hours = atof(arg[1].c_str());
  element = arg[2];
  atomic_number = atoi(arg[3].c_str());
  mass_number = atoi(arg[4].c_str());
  metastable = (std::string(arg[5]) == "Y"? true:false);
  max_beta_minus_energy_in_kev = atof(arg[6].c_str());
}
// --------------------------------------------------


// --------------------------------------------------
double syd::Radionuclide::GetLambdaDecayConstantInHours() const
{
  return log(2.0)/half_life_in_hours;
}
// --------------------------------------------------
