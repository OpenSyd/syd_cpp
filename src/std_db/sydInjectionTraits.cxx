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
template<> void syd::RecordTraits<syd::Injection>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  SetDefaultSortFunctions(map);
  auto f = [](pointer a, pointer b) -> bool { return a->date < b->date; };
  map["date"] = f;
  map[""] = f; // make it the default
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Injection>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  SetDefaultFieldFunctions(map);
  map["date"] = [](pointer a) -> std::string { return a->date; };
  map["activity"]  = [](pointer a) -> std::string { return std::to_string(a->activity_in_MBq); };

  // Prevent to loop if sub-record contains an injection
  static int already_here = false;
  if (already_here) return;
  already_here = true;

  // Build map field for patient and injection
  {
    auto pmap = syd::RecordTraits<syd::Patient>::GetTraits()->GetFieldMap();
    for(auto & m:pmap) {
      std::string s = "patient."+m.first;
      auto f = m.second;
      map[s] = [f](pointer a) -> std::string { return f(a->patient); };
    }
  }
  {
    auto pmap = syd::RecordTraits<syd::Radionuclide>::GetTraits()->GetFieldMap();
    for(auto & m:pmap) {
      std::string s = "radionuclide."+m.first;
      auto f = m.second;
      map[s] = [f](pointer a) -> std::string { return f(a->radionuclide); };
    }
  }
  already_here = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::Injection>::
GetDefaultFields() const
{
  std::string s = "id patient.name radionuclide.name date activity comments";
  return s;
}
// --------------------------------------------------------------------
