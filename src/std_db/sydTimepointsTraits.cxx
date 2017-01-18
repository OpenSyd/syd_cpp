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
#include "sydTimepointsTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Timepoints);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Timepoints>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  // Sort functions from Record
  SetDefaultSortFunctions(map);

  // Contains a RecordHistory, so special case
  syd::RecordWithHistory::CompareFunctionMap m2;
  syd::RecordWithHistory::BuildMapOfSortFunctions(m2);
  map.insert(m2.begin(), m2.end());

  // New sort comparison
  map["date"] = [](pointer a, pointer b) -> bool { return a->injection->date < b->injection->date; };
  map["time"] = [](pointer a, pointer b) -> bool {
    if (a->times.size() == 0 or b->times.size() ==0) return true;
    return a->times[0] < b->times[0]; };
  map[""] = map["date"]; // make this one the default
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Timepoints>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  SetDefaultFieldFunctions(map);

  map["tags"]  = [](pointer a) -> std::string { return syd::GetLabels(a->tags); };
  map["nb"]  = [](pointer a) -> std::string { return syd::ToString(a->times.size()); };
  map["times"] = [](pointer a, int p=1) -> std::string {
    std::ostringstream oss;
    for(auto t:a->times) oss << std::fixed << std::setprecision(p) << t << " ";
    return oss.str(); };

  map["values"] = [](pointer a, int p=2) -> std::string {
    std::ostringstream oss;
    for(auto v:a->values) oss << std::fixed << std::setprecision(p) << v << " ";
    return oss.str(); };

  map["std_dev"] = [](pointer a, int p=2) -> std::string {
    std::ostringstream oss;
    for(auto v:a->std_deviations) oss << std::fixed << std::setprecision(p) << v << " ";
    return oss.str(); };

  // Prevent to loop if sub-record contains an Timepoints
  static int already_here = false;
  if (already_here) {
    LOG(WARNING) << "in TimepointsTraits BuildMapOfFieldsFunctions : loop detected";
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

  already_here = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::Timepoints>::
GetDefaultFields() const
{
  std::string s = "id pat rad tags nb times values";
  return s;
}
// --------------------------------------------------------------------
