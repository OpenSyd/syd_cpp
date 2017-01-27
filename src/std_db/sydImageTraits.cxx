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
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Image);
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Image>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  // Sort functions from Record
  SetDefaultSortFunctions(map);

  // Contains a RecordHistory, so special case
  syd::RecordWithHistory::CompareFunctionMap m2;
  syd::RecordWithHistory::BuildMapOfSortFunctions(m2);
  map.insert(m2.begin(), m2.end());

  // New sort comparison
  auto f = [](pointer a, pointer b) -> bool { return a->acquisition_date < b->acquisition_date; };
  map["date"] = f;
  map[""] = f; // make this one the default
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Image>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  SetDefaultFieldFunctions(map);

#define DEFINE_FIELD_FUNCTION(FIELD)                                \
  map[#FIELD] = [](pointer a) -> std::string { return a->FIELD; };

  DEFINE_FIELD_FUNCTION(type);
  DEFINE_FIELD_FUNCTION(pixel_type);
  DEFINE_FIELD_FUNCTION(frame_of_reference_uid);
  DEFINE_FIELD_FUNCTION(acquisition_date);
  DEFINE_FIELD_FUNCTION(modality);
  map["dimension"]  = [](pointer a) -> std::string { return std::to_string(a->dimension); };
  map["size"]  = [](pointer a) -> std::string { return a->SizeAsString(); };
  map["spacing"]  = [](pointer a) -> std::string { return a->SpacingAsString(); };
  map["tags"]  = [](pointer a) -> std::string { return syd::GetLabels(a->tags); };
  map["files"]  = [](pointer a) -> std::string { return std::to_string(a->files.size()); };
  map["dicoms"]  = [](pointer a) -> std::string {
    if (a->dicoms.size() == 0) return empty_value;
    std::ostringstream oss;
    for (auto & d:a->dicoms) oss << d->id << " ";
    auto s = oss.str();
    return rtrim(s);
  };

  // Prevent to loop if sub-record contains an image
  static int already_here = false;
  if (already_here) {
    LOG(WARNING) << "in ImageTraits BuildMapOfFieldsFunctions : loop detected";
    return;
  }
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
    auto pmap = syd::RecordTraits<syd::Injection>::GetTraits()->GetFieldMap();
    for(auto & m:pmap) {
      std::string s = "injection."+m.first;
      auto f = m.second;
      map[s] = [f](pointer a) -> std::string {
        if (!a->injection) return empty_value;
        return f(a->injection); };
    }
  }

  // Contains a RecordHistory, so special case
  syd::RecordWithComments::FieldFunctionMap m2;
  syd::RecordWithComments::BuildMapOfFieldsFunctions(m2);
  map.insert(m2.begin(), m2.end());
  syd::RecordWithHistory::FieldFunctionMap m3;
  syd::RecordWithHistory::BuildMapOfFieldsFunctions(m3);
  map.insert(m2.begin(), m2.end());

  // Shorter field names
  map["pat"] = map["patient.name"];
  map["rad"] = map["injection.radionuclide.name"];
  map["inj"] = map["injection.id"];
  map["date"] = map["acquisition_date"];

  already_here = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::Image>::
GetDefaultFields() const
{
  std::string s = "id pat date tags rad inj modality size spacing dicoms comments";
  return s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::Image>::
BuildFields(FieldMapType & map) const
{
  DD("image");
  InitCommonFields(map);
  ADD_TABLE_FIELD(patient, syd::Patient);
  ADD_TABLE_FIELD(injection, syd::Injection);
  ADD_TABLE_FIELD(pixel_unit, syd::PixelUnit);

  ADD_FIELD(type, std::string);
  ADD_FIELD(pixel_type, std::string);
  ADD_FIELD(frame_of_reference_uid, std::string);
  ADD_FIELD(acquisition_date, std::string);
  ADD_FIELD(modality, std::string);

  // Read only fields
  ADD_RO_FIELD(dimension, unsigned short int);
  auto f_fn = [](pointer p) -> std::string { return p->GetAbsolutePath(); };
  AddField<std::string>(map, "filename", f_fn);
  auto f_size = [](pointer p) -> std::string { return p->SizeAsString(); };
  AddField<std::string>(map, "size", f_size);
  auto f_spacing = [](pointer p) -> std::string { return p->SpacingAsString(); };
  AddField<std::string>(map, "spacing", f_spacing);

  DD(" trial abbreviation");
  map["pat"] = db_->GetField2("Image", "patient.name");
  map["rad"] = db_->GetField2("Image", "injection.radionuclide.name");

  for(auto m:map) {
    DD(m.first);
    DD(m.second);
  }
}
// --------------------------------------------------------------------
