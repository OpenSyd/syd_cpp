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
#include "sydInjectionTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Injection);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::Injection>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();
  ADD_TABLE_FIELD(patient, syd::Patient);
  ADD_TABLE_FIELD(radionuclide, syd::Radionuclide);
  ADD_FIELD(date, std::string);
  ADD_FIELD(activity_in_MBq, double);

  // Abbreviation
  field_map_["pat"] = db->NewField("Injection", "patient.name", "pat");
  field_map_["rad"] = db->NewField("Injection", "radionuclide.name", "rad");

  field_format_map_["default"] = "id pat rad date activity_in_MBq";
}
// --------------------------------------------------------------------


