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
BuildMapOfSortFunctions(CompareFunctionMap & map)
{
  // Sort functions from Record
  syd::RecordTraits<syd::Record>::CompareFunctionMap m;
  syd::RecordTraits<syd::Record>::BuildMapOfSortFunctions(m);
  map.insert(m.begin(), m.end());

  // New sort comparison
  auto f = [](pointer a, pointer b) -> bool { return a->date < b->date; };
  map["date"] = f;
  map[""] = f; // make it the default
}
// --------------------------------------------------------------------
