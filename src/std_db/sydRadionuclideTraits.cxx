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
template<>
void
syd::RecordTraits<syd::Radionuclide>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();
  ADD_FIELD(name, std::string);
  ADD_FIELD(element, std::string);
  ADD_FIELD_A(atomic_number, double, "Z");
  ADD_FIELD_A(mass_number, double, "A");
  ADD_FIELD(metastable, bool);
  ADD_FIELD_A(half_life_in_hours, double, "hl");
  ADD_FIELD_A(max_beta_minus_energy_in_kev, double, "Q");

  // abbreviation
  field_map_["Q"] = field_map_["max_beta_minus_energy_in_kev"];

  field_format_map_["default"] = "id name element atomic_number mass_number metastable half_life_in_hours max_beta_minus_energy_in_kev";

}
// --------------------------------------------------------------------


