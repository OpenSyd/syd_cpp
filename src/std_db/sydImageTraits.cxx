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
BuildMapOfSortFunctions(CompareFunctionMap & map)
{
  // Inherit from Record and RecordWithHistory, so get additional sort function
  // from those classes.

  // Sort functions from Record
  MergeRecordMapOfSortFunctions(map);

  syd::RecordWithHistory::CompareFunctionMap m2;
  syd::RecordWithHistory::BuildMapOfSortFunctions(m2);
  map.insert(m2.begin(), m2.end());

  // New sort comparison
  auto f = [](pointer a, pointer b) -> bool { return a->acquisition_date < b->acquisition_date; };
  map["date"] = f;
  map[""] = f; // make it the default
}
// --------------------------------------------------------------------
