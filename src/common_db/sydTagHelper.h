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
  class TagHelper
  {
  public:

    /// Update tag list
    template<class ArgsInfo>
    static void UpdateTagsFromCommandLine(syd::Tag::vector & tags,
                                          const syd::Database * db,
                                          ArgsInfo & args_info);

    /// Print the list of tag labels
    static std::string GetLabels(const syd::Tag::vector & tags);

    /// Find a tag by name
    static void FindTags(syd::Tag::vector & tags,
                         const syd::Database * db,
                         const std::string & names);
    static void FindTags(syd::Tag::vector & tags,
                         const syd::Database * db,
                         const std::vector<std::string> & names);
    static void FindTag(syd::Tag::pointer & tag,
                        const syd::Database * db,
                        const std::string & name);

    /// Check if all tags are in the first list
    static bool IsAllTagsIn(syd::Tag::vector & input_tags,
                            syd::Tag::vector & to_search_tags);

    /// manage tag
    static int AddTag(syd::Tag::vector & tags,
                      const syd::Tag::vector & tags_to_add);
    static int AddTag(syd::Tag::vector & tags,
                      const syd::Tag::pointer & tag_to_add);
    static int RemoveTag(syd::Tag::vector & tags,
                         const syd::Tag::vector & tags_to_remove);
    static int RemoveTag(syd::Tag::vector & tags,
                         const syd::Tag::pointer & tag_to_remove);

  }; // end class
} // namespace syd

#include "sydTagHelper.txx"
// --------------------------------------------------------------------

#endif
