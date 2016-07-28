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
#include "sydPrintTable.h"

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
     << (metastable? "metastable ":empty_value)
     << max_beta_minus_energy_in_kev;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Radionuclide::Set(const std::vector<std::string> & arg)
{
  if (arg.size() < 7) {
    DDS(arg);
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
void syd::Radionuclide::DumpInTable(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("name", name);
  ta.Set("HL(h)", half_life_in_hours, 2);
  ta.Set("element", element);
  ta.Set("Z", atomic_number, 0);
  ta.Set("A", mass_number, 0);
  ta.Set("metastable", (metastable? "Y":"N"));
  ta.Set("Q-(keV)", max_beta_minus_energy_in_kev, 2);
}
// --------------------------------------------------


// --------------------------------------------------
double syd::Radionuclide::GetLambdaInHours() const
{
  return log(2.0)/half_life_in_hours;
}
// --------------------------------------------------
