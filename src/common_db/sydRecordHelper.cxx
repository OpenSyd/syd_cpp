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


#include "sydRecordHelper.h"

// --------------------------------------------------------------------
bool syd::ContainsAllTags(const syd::RecordWithTags::pointer & record,
                          const std::vector<std::string> & tag_names)
{
  int n=0;
  for(auto t:tag_names) { // brute force search !!
    auto iter = std::find_if(record->tags.begin(), record->tags.end(),
                             [&t](syd::Tag::pointer & tag)->bool { return tag->label == t;} );
    if (iter == record->tags.end()) continue;
    else ++n;
  }
  if (n == tag_names.size()) return true;
  return false;
}
// --------------------------------------------------------------------
