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

#ifndef SYDTAGHELPER_H
#define SYDTAGHELPER_H

// syd
#include "sydDatabase.h"
#include "sydRecordWithTags.h"

// --------------------------------------------------------------------
namespace syd {

  /// This file contains helpers function that are hopefully helpful
  /// for syd::Tag table

  /// Update tag list
  template<class ArgsInfo>
    static void SetTagsFromCommandLine(syd::Tag::vector & tags,
                                       const syd::Database * db,
                                       ArgsInfo & args_info);

  /// Print the list of tag labels
  std::string GetLabels(const syd::Tag::vector & tags);

  /// Find tags by names (separated with space)
  syd::Tag::vector FindTags(const syd::Database * db,
                            const std::string & names);

  /// Find tags by list of names
  syd::Tag::vector FindTags(const syd::Database * db,
                            const std::vector<std::string> & names);

  /// Find a tag by name
  syd::Tag::pointer FindTag(const syd::Database * db,
                            const std::string & name);

  /// Find a tag by name or create it
  syd::Tag::pointer FindOrCreateTag(syd::Database * db,
                                    const std::string & name,
                                    const std::string & desc="");

  /// Check if all tags are in the first list
  bool IsAllTagsIn(const syd::Tag::vector & input_tags,
                   const syd::Tag::vector & to_search_tags);

  /// Check if all tags are in the first list
  bool IsTagIn(const syd::Tag::vector & input_tags,
               const syd::Tag::pointer & to_search_tag);

  /// Add a tag to the list (check not already exist)
  int AddTag(syd::Tag::vector & tags,
             const syd::Tag::vector & tags_to_add);

  /// Add tags to the list (check not already exist)
  int AddTag(syd::Tag::vector & tags,
             const syd::Tag::pointer & tag_to_add);

  /// Remove tags from the list
  int RemoveTag(syd::Tag::vector & tags,
                const syd::Tag::vector & tags_to_remove);

  /// Remove a tag from the list
  int RemoveTag(syd::Tag::vector & tags,
                const syd::Tag::pointer & tag_to_remove);

  /// Keep only the records if contain all the tags
  template<class RecordType>
    typename RecordType::vector
    GetRecordsThatContainAllTags(const typename RecordType::vector & records,
                                 const std::vector<std::string> & tag_names);

  /// Keep only the records if contain all the tags
  template<class RecordType>
    typename RecordType::vector
    GetRecordsThatContainAllTags(const typename RecordType::vector & records,
                                 const syd::Tag::vector tags);

  /// Keep only the records if contain the tag
  template<class RecordType>
    typename RecordType::vector
    GetRecordsThatContainTag(const typename RecordType::vector & records,
                             const std::string & tag_name);

  /// Keep only the records if contain the tag
  template<class RecordType>
    typename RecordType::vector
    GetRecordsThatContainTag(const typename RecordType::vector & records,
                             const syd::Tag::pointer tag);

  /// Bind vector of records to vector of RecordWithTags. Raise exception if not
  /// possible
  template<class RecordType>
    syd::RecordWithTags::vector
    BindToRecordWithTags(const typename RecordType::vector & records);

} // namespace syd

#include "sydTagHelper.txx"
// --------------------------------------------------------------------

#endif
