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
#include "sydImageTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Image);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Image>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  // Inherit from Record and RecordWithHistory, so get additional sort function
  // from those classes.

  // Sort functions from Record
  SetDefaultSortFunctions(map);

  syd::RecordWithHistory::CompareFunctionMap m2;
  syd::RecordWithHistory::BuildMapOfSortFunctions(m2);
  map.insert(m2.begin(), m2.end());

  // New sort comparison
  auto f = [](pointer a, pointer b) -> bool { return a->acquisition_date < b->acquisition_date; };
  map["date"] = f;
  map[""] = f; // make it the default
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Image>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  DDF();
  DD(map.size());

  SetDefaultFieldFunctions(map);

#define DEFINE_FIELD_FUNCTION(FIELD) \
  map[#FIELD] = [](pointer a) -> std::string { return a->FIELD; };

  DEFINE_FIELD_FUNCTION(modality);
  DEFINE_FIELD_FUNCTION(type);
  DEFINE_FIELD_FUNCTION(pixel_type);
  DEFINE_FIELD_FUNCTION(acquisition_date);
  DEFINE_FIELD_FUNCTION(frame_of_reference_uid);

  // patient ? FIXME --> as a templated function !
  auto pmap = syd::RecordTraits<syd::Patient>::GetTraits()->GetFieldMap();
  for(auto & m:pmap) {
    std::string s = "patient."+m.first;
    DD(s);
    auto f = m.second;
    map[s] = [f](pointer a) -> std::string {
      //if (a->patient == nullptr)
      return f(a->patient);
    };
  }
  DD(map.size());
}
// --------------------------------------------------------------------
