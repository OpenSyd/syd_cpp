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
#include "sydRoiMaskImageTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(RoiMaskImage);
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::RoiMaskImage>::
BuildFields(const syd::Database * db) const
{
  // Retrive fields from image  (inherit)
  auto map = syd::RecordTraits<syd::Image>::GetTraits()->GetFieldsMap(db);
  for(auto & m:map) field_map_[m.first] = m.second->Copy();

  ADD_TABLE_FIELD(roitype, syd::RoiType);

  // Format lists
  field_format_map_["short"] =
    "id pat roitype.name tags rad modality";
  field_format_map_["default"] =
    "id pat roitype.name tags rad injection.id modality size spacing dicoms comments";
}
// --------------------------------------------------------------------
