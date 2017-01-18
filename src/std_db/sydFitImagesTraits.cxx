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
#include "sydFitImagesTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(FitImages);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::FitImages>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  // Sort functions from Record
  SetDefaultSortFunctions(map);

  // Contains a RecordHistory, so special case
  syd::RecordWithHistory::CompareFunctionMap m2;
  syd::RecordWithHistory::BuildMapOfSortFunctions(m2);
  map.insert(m2.begin(), m2.end());
  map[""] = map["update_date"]; // make this one the default
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::FitImages>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  SetDefaultFieldFunctions(map);

  map["images_n"] = [](pointer a) -> std::string { return syd::ToString(a->images.size(),0); };
  map["images"] = [](pointer a) -> std::string {
    std::ostringstream oss;
    for(auto im:a->images) oss << im->id << " ";
    auto s = oss.str();
    return trim(s); };
  map["min_activity"] = [](pointer a, int p=2) -> std::string { return syd::ToString(a->images.size(),p); };
  map["nb_pixels"] = [](pointer a) -> std::string { return syd::ToString(a->nb_pixels,0); };
  map["nb_success_pixels"] = [](pointer a) -> std::string { return syd::ToString(a->nb_success_pixels,0); };
  map["tags"]  = [](pointer a) -> std::string { return syd::GetLabels(a->tags); };
  map["outputs"] = [](pointer a) -> std::string { return a->GetOutputNames(); };
  map["%"] = [](pointer a, int p=2) -> std::string {
    double d = 100.0*(double)a->nb_success_pixels/(double)a->nb_pixels;
    auto s = syd::ToString(d, 2)+"%";
    return s;
  };

  // Patient
  auto pmap = syd::RecordTraits<syd::Patient>::GetTraits()->GetFieldMap();
  for(auto & m:pmap) {
    std::string s = "patient."+m.first;
    auto f = m.second;
    map[s] = [f](pointer a) -> std::string {
      if (a->images.size() == 0) return empty_value;
      return f(a->images[0]->patient); };
  }

  // injection
  auto pmap2 = syd::RecordTraits<syd::Injection>::GetTraits()->GetFieldMap();
  for(auto & m:pmap2) {
    std::string s = "injection."+m.first;
    auto f = m.second;
    map[s] = [f](pointer a) -> std::string {
      if (a->images.size() == 0) return empty_value;
      if (a->images[0]->injection == nullptr) return empty_value;
      return f(a->images[0]->injection); };
  }

 // Contains a RecordWithComments, so special case
  syd::RecordWithComments::FieldFunctionMap m2;
  syd::RecordWithComments::BuildMapOfFieldsFunctions(m2);
  map.insert(m2.begin(), m2.end());
  syd::RecordWithHistory::FieldFunctionMap m3;
  syd::RecordWithHistory::BuildMapOfFieldsFunctions(m3);
  map.insert(m2.begin(), m2.end());
  syd::FitOptions::FieldFunctionMap m4;
  syd::FitOptions::BuildMapOfFieldsFunctions(m4);
  map.insert(m4.begin(), m4.end());

  // Shorter field names
  map["pat"] = map["patient.name"];
  map["n"] = map["nb_pixels"];
  map["mina"] = map["min_activity"];
  map["rad"] = map["injection.radionuclide.name"];
  // map["inj"] = map["injection.id"];
  // map["date"] = map["acquisition_date"];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::FitImages>::
GetDefaultFields() const
{
  std::string s = "id pat rad images models mina r2_min ak rest it n % outputs";
  return s;
}
// --------------------------------------------------------------------