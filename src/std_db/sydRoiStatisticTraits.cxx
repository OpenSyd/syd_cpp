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
#include "sydRoiStatisticTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(RoiStatistic);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::RoiStatistic>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  // Sort functions from Record
  SetDefaultSortFunctions(map);

  // Contains a RecordHistory, so special case
  syd::RecordWithHistory::CompareFunctionMap m2;
  syd::RecordWithHistory::BuildMapOfSortFunctions(m2);
  map.insert(m2.begin(), m2.end());

  // New sort comparison
  auto f = [](pointer a, pointer b) ->
    bool { return a->image->acquisition_date < b->image->acquisition_date; };
  map["date"] = f;
  map[""] = f; // make this one the default
  map["mask"]  = [](pointer a, pointer b) -> bool {
    if (a->mask == nullptr or b->make == nullptr) return true;
    return a->mask->roitype->name < b->mask->roitype->name; };
  map["mean"]  = [](pointer a, pointer b) -> bool { return a->mean < b->mean; };
  map["n"]  = [](pointer a, pointer b) -> bool { return a->n < b->n; };
  map["sum"]  = [](pointer a, pointer b) -> bool { return a->sum < b->sum; };
  map["std_dev"]  = [](pointer a, pointer b) -> bool { return a->std_dev < b->std_dev; };
  map["min"]  = [](pointer a, pointer b) -> bool { return a->min < b->min; };
  map["max"]  = [](pointer a, pointer b) -> bool { return a->max < b->max; };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::RoiStatistic>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  SetDefaultFieldFunctions(map);

  map["date"] = [](pointer a) -> std::string { return a->image->acquisition_date; };
  map["tags"]  = [](pointer a) -> std::string { return syd::GetLabels(a->image->tags); };
  map["mean"] = [](pointer a, int p=2) -> std::string { return std::ToString(a->mean,p); };

  // Prevent to loop if sub-record contains an RoiStatistic
  static int already_here = false;
  if (already_here) {
    LOG(WARNING) << "in RoiStatisticTraits BuildMapOfFieldsFunctions : loop detected";
    return;
  }
  already_here = true;

  // Build map field for patient and injection
  {
    auto pmap = syd::RecordTraits<syd::Image>::GetTraits()->GetFieldMap();
    for(auto & m:pmap) {
      std::string s = "image."+m.first;
      auto f = m.second;
      map[s] = [f](pointer a) -> std::string { return f(a->image); };
    }
  }
  {
    auto pmap = syd::RecordTraits<syd::mask>::GetTraits()->GetFieldMap();
    for(auto & m:pmap) {
      std::string s = "mask."+m.first;
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
  already_here = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::RoiStatistic>::
GetDefaultFields() const
{
  std::string s = "id image.patient.name mask.roitype.name image.id tags mean std_dev n min max sum";
  return s;
}
// --------------------------------------------------------------------
