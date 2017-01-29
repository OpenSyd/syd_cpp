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
#include "sydRadionuclideTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Radionuclide);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Radionuclide>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  SetDefaultSortFunctions(map);
  map["name"] = [](pointer a, pointer b) -> bool { return a->name < b->name; };
  map["element"] = [](pointer a, pointer b) -> bool { return a->name < b->element; };
  map["half_life"] = [](pointer a, pointer b) -> bool { return a->half_life_in_hours < b->half_life_in_hours; };
  map["Z"] = [](pointer a, pointer b) -> bool { return a->atomic_number < b->atomic_number; };
  map["A"] = [](pointer a, pointer b) -> bool { return a->mass_number < b->mass_number; };
  map[""] = map["Z"]; // default
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Radionuclide>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  SetDefaultFieldFunctions(map);

  map["name"] = [](pointer a) -> std::string { return a->name; };
  map["element"] = [](pointer a) -> std::string { return a->element; };
  map["Z"] = [](pointer a) -> std::string { return syd::ToString(a->atomic_number,0); };
  map["A"] = [](pointer a) -> std::string { return syd::ToString(a->mass_number, 0); };
  map["metastable"] = [](pointer a) -> std::string { return std::to_string(a->metastable); };
  map["half_life"] = [](pointer a) -> std::string { return syd::ToString(a->half_life_in_hours,2); };
  map["Q-"] = [](pointer a) -> std::string { return syd::ToString(a->max_beta_minus_energy_in_kev,2); };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::Radionuclide>::
GetDefaultFields() const
{
  std::string s = "id name element Z A metastable half_life Q-";
  return s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::Radionuclide>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();
  ADD_FIELD(name, std::string);
  // ADD_TABLE_FIELD(radionuclide, syd::Radionuclide);
  // ADD_FIELD(date, std::string);
  // ADD_FIELD(activity_in_MBq, double);
}
// --------------------------------------------------------------------


