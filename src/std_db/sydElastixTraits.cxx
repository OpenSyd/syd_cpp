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
#include "sydElastixTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Elastix);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Elastix>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  // Sort functions from Record
  SetDefaultSortFunctions(map);
  // Contains a RecordHistory, so special case
  syd::RecordWithHistory::CompareFunctionMap m2;
  syd::RecordWithHistory::BuildMapOfSortFunctions(m2);
  map.insert(m2.begin(), m2.end());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Elastix>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  SetDefaultFieldFunctions(map);

  map["tags"]  = [](pointer a) -> std::string { return syd::GetLabels(a->tags); };

  // Build map field for moving_image
  {
    auto pmap = syd::RecordTraits<syd::Image>::GetTraits()->GetFieldMap();
    for(auto & m:pmap) {
      std::string s = "moving_image."+m.first;
      auto f = m.second;
      map[s] = [f](pointer a) -> std::string { return f(a->moving_image); };
    }
  }
  {
    auto pmap = syd::RecordTraits<syd::Image>::GetTraits()->GetFieldMap();
    for(auto & m:pmap) {
      std::string s = "fixed_image."+m.first;
      auto f = m.second;
      map[s] = [f](pointer a) -> std::string { return f(a->fixed_image); };
    }
  }
  {
    auto pmap = syd::RecordTraits<syd::RoiMaskImage>::GetTraits()->GetFieldMap();
    for(auto & m:pmap) {
      std::string s = "fixed_mask."+m.first;
      auto f = m.second;
      map[s] = [f](pointer a) -> std::string {
        if (a->fixed_mask == nullptr) return empty_value;
        return f(a->fixed_mask); };
    }
  }
  {
    auto pmap = syd::RecordTraits<syd::RoiMaskImage>::GetTraits()->GetFieldMap();
    for(auto & m:pmap) {
      std::string s = "moving_mask."+m.first;
      auto f = m.second;
      map[s] = [f](pointer a) -> std::string {
        if (a->moving_mask == nullptr) return empty_value;
        return f(a->moving_mask); };
    }
  }
  {
    auto pmap = syd::RecordTraits<syd::File>::GetTraits()->GetFieldMap();
    for(auto & m:pmap) {
      std::string s = "config_file."+m.first;
      auto f = m.second;
      map[s] = [f](pointer a) -> std::string {
        if (a->config_file == nullptr) return empty_value;
        return f(a->config_file); };
    }
  }
  {
    auto pmap = syd::RecordTraits<syd::File>::GetTraits()->GetFieldMap();
    for(auto & m:pmap) {
      std::string s = "transform_file."+m.first;
      auto f = m.second;
      map[s] = [f](pointer a) -> std::string {
        if (a->transform_file == nullptr) return empty_value;
        return f(a->transform_file); };
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
  map["pat"] = map["fixed_image.patient.name"];
  map["date"] = map["fixed_image.acquisition_date"];
  map["config"] = map["config_file.filename"];
  map["result"] = map["transform_file.path"];
  map["f"] = map["fixed_image.id"];
  map["m"] = map["moving_image.id"];
  map["mf"] = map["fixed_mask.roi.name"];
  map["mm"] = map["moving_mask.roi.name"];
  map["fframe"] = map["fixed_mask.frame_of_reference_uid"];
  map["mframe"] = map["moving_mask.frame_of_reference_uid"];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::Elastix>::
GetDefaultFields() const
{
  std::string s = "id pat date tags config result f m mf mm fframe comments";
  return s;
}
// --------------------------------------------------------------------
