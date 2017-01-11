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

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Image);
// --------------------------------------------------------------------


template<> void syd::RecordTraits<syd::Image>::
BuildMapOfSortFunctions(CompareFunctionMap & map)
{
  DDF();
  /*
    auto t = syd::RecordTraits<syd::Record>::GetTraits();
    auto tt = std::static_cast<syd::RecordTraits<syd::Record>>(t);
    auto map = tt->GetMapOfSortFunctions();
    DD(map.size());
    map_of_sort_functions.insert(map.begin(), map.end());
  */
  DD(map.size());

  /*
  //GetTraits()->BuildMapOfSortFunctions<syd::Record>();
  map_of_sort_functions["date"] = [](syd::Image::vector & v) {
  std::sort(begin(v), end(v), [v](pointer a, pointer b) {
  return a->acquisition_date < b->acquisition_date; });
  };
  */

  typedef std::function<bool(syd::Record::pointer a, syd::Record::pointer b)> CompareFunctionT;
  typedef std::map<std::string, CompareFunctionT> CompareFunctionMapT;
  CompareFunctionMapT m;
  syd::RecordTraits<syd::Record>::BuildMapOfSortFunctions(m);

  map.insert(m.begin(), m.end());

  map["date"] =
    [](pointer a, pointer b) -> bool { return a->acquisition_date < b->acquisition_date; };
  DD(map.size());


  DD("trial id ?");

  //auto t = syd::RecordTraits<syd::Record>::GetTraits();
  //  auto map = syd::RecordTraits<syd::Record>::GetMapOfSortFunctions();

  //  map_of_sort_functions["id"] = [](syd::Record::pointer a, syd::Record::pointer b) -> bool { return a->id < b->id; };

  DD(map.size());

  // auto f = [](syd::Record::vector & v) {
  //   std::sort(begin(v), end(v), [v](pointer a, pointer b) {
  //       return a->id < b->id; });
  // };
  // map_of_sort_functions["id"] = f;
}

// --------------------------------------------------------------------
/*
  template<> void syd::RecordTraits<syd::Image>::
  Sort(syd::Image::vector & v, const std::string & type) const
  {
  DD("specific Image sort");
*/
/*
  auto it = map_of_sort_functions.find("date");
  if (it == map_of_sort_functions.end()) {
  map_of_sort_functions["date"] = [](syd::Image::vector v) {
  std::sort(begin(v), end(v), [v](pointer a, pointer b) {
  return a->acquisition_date < b->acquisition_date; });
  };
  }
  syd::RecordTraits<syd::Record>::GetTraits()->Sort(db, temp, type);
*/
  
/*

  if (type == "default" or type=="date" or type=="") {
  std::sort(begin(v), end(v), [v](pointer a, pointer b) {
  return a->acquisition_date < b->acquisition_date; });
  //    return true;
  }
  DD("not default");
*/
/*
  auto temp = syd::ConvertToVectorOfRecords(v);
  auto b = syd::RecordTraits<syd::RecordWithHistory>::GetTraits()->Sort(db, temp, type);
  if (b) {
  v = syd::CastFromVectorOfRecords<syd::RoiMaskImage>(temp);
  return true;
  }
  if (type == "help") {
  LOG(0) << "Available sort type: 'id' or 'date'";
  }
*/
//}

// --------------------------------------------------------------------
