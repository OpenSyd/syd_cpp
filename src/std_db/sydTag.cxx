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
#include "sydTag.h"

// std
#include <set>

// --------------------------------------------------
syd::Tag::Tag():syd::RecordWithHistory()
{
  label = "unset";
  description = "unset";
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::Tag::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << label << " "
     << description;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Tag::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and label == p->label and
          description == p->description);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Tag::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "To insert a Tag, please set <label> <description>";
  }
  label = arg[0];
  description = arg[1];
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Tag::InitTable(syd::PrintTable & ta) const
{
  syd::RecordWithHistory::InitTable(ta);
  if (ta.GetColumn("id") == -1) ta.AddColumn("id");
  ta.AddColumn("label");
  auto & col = ta.AddColumn("description");
  col.max_width = 80;
  col.trunc_by_end_flag = true;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Tag::DumpInTable(syd::PrintTable & ta) const
{
  syd::RecordWithHistory::DumpInTable(ta);
  ta.Set("label", label);
  ta.Set("description", description);
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::GetLabels(const syd::Tag::vector & tags)
{
  std::ostringstream os;
  if (tags.size() == 0) return "no_tag";
  os << tags[0]->label;
  for(auto i=1; i<tags.size(); i++) os << "," << tags[i]->label;
  return os.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::IsAllTagsIn(syd::Tag::vector & input_tags, syd::Tag::vector & to_search_tags)
{
  // http://stackoverflow.com/questions/5225820/compare-two-vectors-c
  std::set<syd::Tag::pointer> s1(input_tags.begin(), input_tags.end());
  std::set<syd::Tag::pointer> s2(to_search_tags.begin(), to_search_tags.end());
  std::vector<syd::Tag::pointer> v3;
  std::set_intersection(s1.begin(), s1.end(),
                        s2.begin(), s2.end(),
                        std::back_inserter(v3),
                        [](const syd::Tag::pointer a, const syd::Tag::pointer b) { return a->label > b->label; } );
  return (v3.size() == to_search_tags.size());
}
// --------------------------------------------------
