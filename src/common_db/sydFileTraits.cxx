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
#include "sydFileTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(File);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::File>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  // Sort functions from Record
  SetDefaultSortFunctions(map);
  // New sort comparison
  auto f = [](pointer a, pointer b) -> bool { return a->filename < b->filename; };
  map["filename"] = f;
  map[""] = f; // make this one the default
  map["path"] = [](pointer a, pointer b) -> bool { return a->path < b->path; };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::File>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  SetDefaultFieldFunctions(map);
  map["filename"] = [](pointer a) -> std::string { return a->filename; };
  map["path"] = [](pointer a) -> std::string { return a->path; };
  map["md5"] = [](pointer a) -> std::string { return a->md5; };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::File>::
GetDefaultFields() const
{
  std::string s = "id filename path md5";
  return s;
}
// --------------------------------------------------------------------
