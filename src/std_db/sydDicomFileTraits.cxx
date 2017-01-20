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
#include "sydDicomFileTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(DicomFile);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::DicomFile>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  // Sort functions from File
  syd::RecordTraits<syd::File>::CompareFunctionMap m;
  syd::RecordTraits<syd::File>::GetTraits()->BuildMapOfSortFunctions(m);
  map.insert(m.begin(), m.end());

  // New sort comparison
  auto f = [](pointer a, pointer b) -> bool { return a->dicom_sop_uid < b->dicom_sop_uid; };
  map["dicom_sop_uid"] = f;
  map[""] = f; // make this one the default
  map["dicom_instance_number"] = [](pointer a, pointer b) ->
    bool { return a->dicom_instance_number < b->dicom_instance_number; };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::DicomFile>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  // Inherit from File
  syd::RecordTraits<syd::File>::FieldFunctionMap m;
  syd::RecordTraits<syd::File>::GetTraits()->BuildMapOfFieldsFunctions(m);
  map.insert(m.begin(), m.end());
  map["dicom_sop_uid"] = [](pointer a) -> std::string { return a->dicom_sop_uid; };
  map["dicom_instance_number"] = [](pointer a) -> std::string { return syd::ToString(a->dicom_instance_number,0); };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::DicomFile>::
GetDefaultFields() const
{
  std::string s = "id filename dicom_instance_number dicom_sop_uid path";
  return s;
}
// --------------------------------------------------------------------
