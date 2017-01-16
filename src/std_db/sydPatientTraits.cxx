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
#include "sydPatientTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Patient);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Patient>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  // Sort functions from Record
  SetDefaultSortFunctions(map);

  // New sort comparison
  map["name"] = [](pointer a, pointer b) -> bool { return a->name < b->name; };
  auto f = [](pointer a, pointer b) -> bool { return a->study_id < b->study_id; };
  map["study_id"] = f;
  map[""] = f; // make it the default
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Patient>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  DDF();
  DD(map.size());

  SetDefaultFieldFunctions(map);

#define DEFINE_FIELD_FUNCTION(FIELD) \
  map[#FIELD] = [](pointer a) -> std::string { return a->FIELD; };

  DEFINE_FIELD_FUNCTION(name);
  //  DEFINE_FIELD_FUNCTION(study_id);
  DEFINE_FIELD_FUNCTION(dicom_patientid);
  DEFINE_FIELD_FUNCTION(sex);

  DD(map.size());
}
// --------------------------------------------------------------------