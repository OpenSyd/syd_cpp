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

#ifndef SYDTAG_H
#define SYDTAG_H

// syd
#include "sydTableElementBase.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object
  /// Simple table to store a label and a description
  class Tag : public syd::TableElementBase {
  public:

#pragma db id auto
    /// Id of the Tag
    IdType id;

#pragma db options("UNIQUE")
    /// Label of the tag (name)
    std::string label;

    /// Description associated with the tag
    std::string description;

    // ------------------------------------------------------------------------
    SET_TABLE_NAME("Tag")
    Tag();

    virtual std::string ToString() const;
    virtual void Set(std::vector<std::string> & arg);

    bool operator==(const Tag & p);
    bool operator!=(const Tag & p) { return !(*this == p); }

  }; // end class


  // Helpers function
  std::string GetTagLabels(const std::vector<std::shared_ptr<syd::Tag>> & tags);

}
// --------------------------------------------------------------------

#endif
