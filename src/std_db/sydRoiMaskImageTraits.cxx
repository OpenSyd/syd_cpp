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
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  // Like image
  syd::RecordTraits<syd::Image>::CompareFunctionMap m;
  syd::RecordTraits<syd::Image>::GetTraits()->BuildMapOfSortFunctions(m);
  map.insert(m.begin(), m.end());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::RoiMaskImage>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  // From image
  syd::RecordTraits<syd::Image>::FieldFunctionMap m;
  syd::RecordTraits<syd::Image>::GetTraits()->BuildMapOfFieldsFunctions(m);
  map.insert(m.begin(), m.end());
  // add roitype
  auto pmap = syd::RecordTraits<syd::RoiType>::GetTraits()->GetFieldMap();
  for(auto & m:pmap) {
    std::string s = "roi."+m.first;
    auto f = m.second;
    map[s] = [f](pointer a) -> std::string { return f(a->roitype); };
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::RoiMaskImage>::
GetDefaultFields() const
{
  std::string s = "id patient.name roi.name tags modality size spacing dicoms comments";
  return s;
}
// --------------------------------------------------------------------
