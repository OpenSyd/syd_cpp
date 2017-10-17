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
#include "sydTagHelper.h"
#include "sydRecordHelper.h"

// --------------------------------------------------------------------
syd::Tag::vector syd::FindTags(const syd::Database * db,
                               const std::string & names)
{
  syd::Tag::vector tags;
  std::vector<std::string> words;
  syd::GetWords(words, names);
  return syd::FindTags(db, words);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Tag::pointer syd::FindTag(const syd::Database * db,
                               const std::string & name)
{
  syd::Tag::vector tags = syd::FindTags(db, name);
  if (tags.size() != 1) {
    EXCEPTION("Error in FindTag '" << name << "', I find "
              << tags.size() << " tags");
  }
  return tags[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Tag::pointer syd::FindOrCreateTag(syd::Database * db,
                                       const std::string & name,
                                       const std::string & desc)
{
  syd::Tag::pointer tag;
  try {
    tag = syd::FindTag(db, name);
  } catch (std::exception & e) {
    tag = db->New<syd::Tag>();
    tag->label = name;
    tag->description = desc;
    db->Insert(tag);
  }
  return tag;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Tag::vector syd::FindTags(const syd::Database * db,
                               const std::vector<std::string> & names)
{
  syd::Tag::vector tags;
  odb::query<Tag> q = odb::query<Tag>::label.in_range(names.begin(), names.end());
  db->Query<Tag>(tags, q);
  if (tags.size() != names.size()) {
    std::string s;
    for(auto t:tags) s += t->label+" ";
    std::string w;
    syd::SetWords(w, names);
    EXCEPTION("Cannot find all tags in FindTags. Look for: '" << w
              << "' but find: '" << s << "'");
  }
  return tags;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::IsAllTagsIn(const syd::Tag::vector & input_tags,
                      const syd::Tag::vector & to_search_tags)
{
  /*
  // http://stackoverflow.com/questions/5225820/compare-two-vectors-c
  std::set<syd::Tag::pointer> s1(input_tags.begin(), input_tags.end());
  std::set<syd::Tag::pointer> s2(to_search_tags.begin(), to_search_tags.end());
  std::vector<syd::Tag::pointer> v3;
  std::set_intersection(s1.begin(), s1.end(),
  s2.begin(), s2.end(),
  std::back_inserter(v3),
  [](const syd::Tag::pointer a, const syd::Tag::pointer b) {
  return a->id < b->id; } );
  return (v3.size() == to_search_tags.size());
  */
  for(auto tag_to_search:to_search_tags) {
    if (!syd::IsTagIn(input_tags, tag_to_search)) return false;
  }
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::IsTagIn(const syd::Tag::vector & input_tags,
                  const syd::Tag::pointer & to_search_tag)
{
  for(auto & t:input_tags)
    if (t->id == to_search_tag->id) return true;
  return false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::AddTag(syd::Tag::vector & tags,
                const syd::Tag::vector & tags_to_add)
{
  int n = 0;
  for(auto t:tags_to_add) n += AddTag(tags, t);
  return n;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::AddTag(syd::Tag::vector & tags,
                const syd::Tag::pointer & tag_to_add)
{
  // (rather use a set instead of a tag to keep it ordered ?)
  bool found = false;
  int i=0;
  while (i<tags.size() and !found) {
    if (tags[i]->id == tag_to_add->id) found = true;
    ++i;
  }
  if (!found) {
    tags.push_back(tag_to_add);
    return 1;
  }
  else return 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::RemoveTag(syd::Tag::vector & tags,
                   const syd::Tag::vector & tags_to_remove)
{
  int n = 0;
  for(auto t:tags_to_remove) n += RemoveTag(tags, t);
  return n;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::RemoveTag(syd::Tag::vector & tags,
                   const syd::Tag::pointer & tag_to_remove)
{
  // (rather use a set instead of a tag to keep it ordered ?)
  int i=0;
  while (i<tags.size()) {
    if (tags[i]->id == tag_to_remove->id) {
      tags.erase(tags.begin()+i);
      return 1;
    }
    ++i;
  }
  return 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GetLabels(const syd::Tag::vector & tags)
{
  std::ostringstream os;
  if (tags.size() == 0) return empty_value;
  os << tags[0]->label;
  for(auto i=1; i<tags.size(); i++) os << " " << tags[i]->label;
  return os.str();
}
// --------------------------------------------------------------------


