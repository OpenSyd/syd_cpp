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

// --------------------------------------------------------------------
template<class ArgsInfo>
void syd::SetTagsFromCommandLine(syd::Tag::vector & tags,
                                 const syd::Database * db,
                                 ArgsInfo & args_info)
{
  // Remove all tags
  if (args_info.remove_all_tag_flag) tags.clear();

  // Remove some tags
  if (args_info.remove_tag_given) {
    for(auto i=0; i<args_info.remove_tag_given; i++) {
      std::string tagname = args_info.remove_tag_arg[i];
      syd::Tag::vector tags_temp;
      try {
        tags_temp = syd::FindTags(db, tagname);
      } catch(std::exception & e) {
        LOG(WARNING) << "Cannot remove unknown tag '" << tagname << "'.";
      } // ignore unknown tag
      syd::RemoveTag(tags, tags_temp);
    }
  }

  // Add tags
  if (args_info.tag_given) {
    for(auto i=0; i<args_info.tag_given; i++) {
      std::string tagname = args_info.tag_arg[i];
      syd::Tag::vector tags_temp;
      try {
        tags_temp = syd::FindTags(db, tagname);
      } catch(std::exception & e) {
        LOG(WARNING) << "Some tags were not found. " << e.what();
      }
      syd::AddTag(tags, tags_temp);
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename RecordType::vector
syd::GetRecordsThatContainTag(const typename RecordType::vector & records,
                              const std::string & tag_name)
{
  if (records.size() == 0) return records;
  syd::Tag::pointer tag = syd::FindTag(records[0]->GetDatabase(), tag_name);
  return GetRecordsThatContainAllTags<RecordType>(records, tag);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename RecordType::vector
syd::GetRecordsThatContainAllTags(const typename RecordType::vector & records,
                                  const std::vector<std::string> & tag_names)
{
  if (records.size() == 0) return records;
  syd::Tag::vector tags = syd::FindTags(records[0]->GetDatabase(), tag_names);
  return GetRecordsThatContainAllTags<RecordType>(records, tags);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename RecordType::vector
syd::GetRecordsThatContainTag(const typename RecordType::vector & records,
                              const syd::Tag::pointer tag)
{
  syd::Tag::vector tags;
  tags.push_back(tag);
  return syd::GetRecordsThatContainAllTags<RecordType>(records, tags);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename RecordType::vector
syd::GetRecordsThatContainAllTags(const typename RecordType::vector & records,
                                  const syd::Tag::vector tags)
{
  // Loop on records with tags
  typename RecordType::vector results;
  for(auto record:records) {
    auto x = std::dynamic_pointer_cast<syd::RecordWithTags>(record);
    if (x == nullptr) {
      EXCEPTION("The record does not have tags. "
                "Cannot use the function GetRecordsThatContainAllTags");
    }
    if (syd::IsAllTagsIn(x->tags, tags)) {
      results.push_back(record);
    }
  }
  return results;
}
// --------------------------------------------------------------------



//--------------------------------------------------------------------
template<class RecordType>
syd::RecordWithTags::vector
syd::BindToRecordWithTags(const typename RecordType::vector & records)
{
  syd::RecordWithTags::vector records_with_tags;
  for(auto record:records) {
    auto r = std::dynamic_pointer_cast<syd::RecordWithTags>(record);
    if (r == nullptr) {
      EXCEPTION("Error, the records of table " << record->GetTableName()
                << " do not have tags");
    }
    records_with_tags.push_back(r);
  }
  return records_with_tags;
}
//--------------------------------------------------------------------
