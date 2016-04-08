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

#ifndef SYDRECORDWITHTAGS_H
#define SYDRECORDWITHTAGS_H

// syd
#include "sydTag.h"

// --------------------------------------------------------------------
namespace syd {

  //#pragma db object abstract pointer(std::shared_ptr)
  //callback(Callback)
  class RecordWithTags  {
  public:

    /// List of tags
    syd::Tag::vector tags;

    // Add tag, remove tag, print etc


  protected:
    RecordWithTags();

  };

} // end namespace
// --------------------------------------------------------------------

#endif
