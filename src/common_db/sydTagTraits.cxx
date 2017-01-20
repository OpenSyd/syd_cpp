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
template<> void syd::RecordTraits<syd::Tag>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  DDF();
  DD("tag");
  // Sort functions from Record
  SetDefaultSortFunctions(map);
  // New sort comparison
  auto f = [](pointer a, pointer b) -> bool { return a->label < b->label; };
  map["label"] = f;
  map[""] = f; // make this one the default
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Tag>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  DDF();
  DD("tag");
  SetDefaultFieldFunctions(map);
  map["label"] = [](pointer a) -> std::string { return a->label; };
  map["description"] = [](pointer a) -> std::string { return a->description; };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::Tag>::
GetDefaultFields() const
{
  std::string s = "id label description";
  return s;
}
// --------------------------------------------------------------------
