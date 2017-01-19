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
#include "sydDicomSerieTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(DicomSerie);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::DicomSerie>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  SetDefaultSortFunctions(map);
  // New sort comparison
  auto f = [](pointer a, pointer b) -> bool
    { return a->dicom_acquisition_date < b->dicom_acquisition_date; };
  map["date"] = f;
  map[""] = f; // make it the default
  map["reconstruction_date"] = [](pointer a, pointer b) -> bool
    { return a->dicom_reconstruction_date < b->dicom_reconstruction_date; };
  map["acquisition_date"] = [](pointer a, pointer b) -> bool
    { return a->dicom_acquisition_date < b->dicom_acquisition_date; };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::DicomSerie>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  SetDefaultFieldFunctions(map);

  map["acquisition_date"] = [](pointer a) -> std::string { return a->dicom_acquisition_date; };
  map["reconstruction_date"] = [](pointer a) -> std::string { return a->dicom_reconstruction_date; };
  map["dicom_study_uid"] = [](pointer a) -> std::string { return a->dicom_study_uid; };
  map["dicom_serie_uid"] = [](pointer a) -> std::string { return a->dicom_series_uid; };
  map["dicom_frame_of_reference_uid"] = [](pointer a) -> std::string { return a->dicom_frame_of_reference_uid; };
  map["dicom_modality"] = [](pointer a) -> std::string { return a->dicom_modality; };
  map["dicom_description"] = [](pointer a) -> std::string { return a->dicom_description; };

  map["dicom_files"] = [](pointer a) -> std::string { return syd::ToString(a->dicom_files.size(),0); };

  auto pmap = syd::RecordTraits<syd::Patient>::GetTraits()->GetFieldMap();
  for(auto & m:pmap) {
    std::string s = "patient."+m.first;
    auto f = m.second;
    map[s] = [f](pointer a) -> std::string { return f(a->patient); };
  }

  map["date"] = map["acquisition_date"];
  map["pat"] = map["patient.name"];
  map["files"] = map["dicom_files"];
  map["mod"] = map["dicom_modality"];
  map["description"] = map["dicom_description"];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::DicomSerie>::
GetDefaultFields() const
{
  std::string s = "id pat date files mod dicom_description reconstruction_date";
  return s;
}
// --------------------------------------------------------------------
