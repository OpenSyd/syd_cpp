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
#include "sydTagTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Tag);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::Tag>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();
  ADD_FIELD(label, std::string);
  ADD_FIELD(description, std::string);

  // Format lists
  field_format_map_["default"] = "id label description";
}
// --------------------------------------------------------------------
