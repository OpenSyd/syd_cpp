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
#include "sydFitTimepointsTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(FitTimepoints);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::FitTimepoints>::
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
template<> void syd::RecordTraits<syd::FitTimepoints>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  SetDefaultFieldFunctions(map);

  map["tags"]  = [](pointer a) -> std::string { return syd::GetLabels(a->tags); };
  map["params"]  = [](pointer a, int p=2) -> std::string {
    std::ostringstream oss;
    for(auto pa:a->params) oss << std::fixed << std::setprecision(p) << pa << " ";
    return oss.str();
  };
  map["auc"]  = [](pointer a, int p=2) -> std::string { return syd::ToString(a->auc, p); };
  map["r2"]  = [](pointer a, int p=2) -> std::string { return syd::ToString(a->r2, p); };
  map["first_index"]  = [](pointer a) -> std::string { return syd::ToString(a->first_index, 0); };
  map["iterations"]  = [](pointer a) -> std::string { return syd::ToString(a->iterations, 0); };
  map["model"]  = [](pointer a) -> std::string { return a->model_name; };

  // FitOptions
  syd::FitOptions::FieldFunctionMap m4;
  syd::FitOptions::BuildMapOfFieldsFunctions(m4);
  map.insert(m4.begin(), m4.end());

  // Timepoints
  auto pmap = syd::RecordTraits<syd::Timepoints>::GetTraits()->GetFieldMap();
  for(auto & m:pmap) {
    std::string s = "timepoints."+m.first;
    auto f = m.second;
    map[s] = [f](pointer a) -> std::string { return f(a->timepoints); };
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::FitTimepoints>::
GetDefaultFields() const
{
  std::string s = "id timepoints.id timepoints.pat timepoints.rad timepoints.nb tags model auc r2 first_index iterations params r2_min ak rest it models";
  return s;
}
// --------------------------------------------------------------------
