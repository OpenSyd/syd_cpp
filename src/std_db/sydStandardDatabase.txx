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
/*template<class ArgsInfo>
void syd::StandardDatabase::UpdateTagsFromCommandLine(syd::Tag::vector & tags,
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
        tags_temp = syd::TagHelper::FindTags(this, tagname);
      } catch(std::exception & e) { } // ignore unknown tag
      RemoveTag(tags, tags_temp);
    }
  }

  // Add tags
  if (args_info.tag_given) {
    for(auto i=0; i<args_info.tag_given; i++) {
      std::string tagname = args_info.tag_arg[i];
      syd::Tag::vector tags_temp;
      try {
        FindTags(tags_temp, tagname);
      } catch(std::exception & e) {
        LOG(WARNING) << "Some tags are ignored. " << e.what();
      }
      AddTag(tags, tags_temp);
    }
  }
}
*/
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
bool syd::StandardDatabase::FindSameMD5(const typename RecordType::pointer input,
                                        typename RecordType::pointer & output)
{
  auto m = input->ComputeMD5();
  odb::query<RecordType> q = odb::query<RecordType>::md5 == m;
  try {
    QueryOne(output, q);
  } catch (std::exception & e) {
    return false;
  }
  return true;
}
// --------------------------------------------------------------------
