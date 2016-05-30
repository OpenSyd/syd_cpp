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

#pragma db object abstract pointer(std::shared_ptr)
  class RecordWithTags  {
  public:

    /// Define pointer type
    typedef std::shared_ptr<RecordWithTags> pointer;

    /// Define vectortype
    typedef std::vector<pointer> vector;

    /// List of tags
    syd::Tag::vector tags;

    /// Copy list of tags
    void CopyFrom(const syd::RecordWithTags::pointer & from);

  protected:
    RecordWithTags();

  };

} // end namespace
// --------------------------------------------------------------------

#endif
