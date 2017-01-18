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
#include "sydRoiTimepointsTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(RoiTimepoints);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::RoiTimepoints>::
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
template<> void syd::RecordTraits<syd::RoiTimepoints>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  // From timepoints
  syd::RecordTraits<syd::Timepoints>::FieldFunctionMap m;
  syd::RecordTraits<syd::Timepoints>::GetTraits()->BuildMapOfFieldsFunctions(m);
  map.insert(m.begin(), m.end());

  auto pmap = syd::RecordTraits<syd::RoiStatistic>::GetTraits()->GetFieldMap();
  for(auto & m:pmap) {
    std::string s = "roi."+m.first;
    auto f = m.second;
    map[s] = [f](pointer a) -> std::string {
      if (a->roi_statistics.size() == 0) return empty_value;
      return f(a->roi_statistics[0]); };
  }

  map["stats"] = [](pointer a) -> std::string {
    std::ostringstream oss;
    for(auto r:a->roi_statistics) oss << r->id << " ";
    auto s = oss.str();
    return trim(s); };

  // Shorter field names
  map["mask"] = map["roi.mask"];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::RoiTimepoints>::
GetDefaultFields() const
{
  std::string s = "id pat mask rad tags nb times values stats";
  return s;
}
// --------------------------------------------------------------------
