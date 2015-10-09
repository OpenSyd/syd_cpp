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

#ifndef SYDPIXELVALUEUNIT_H
#define SYDPIXELVALUEUNIT_H

// syd
#include "sydRecord.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::PixelValueUnit")
  /// Simple table to store a label and a description
  class PixelValueUnit : public syd::Record {
  public:

#pragma db options("UNIQUE")
    /// Label of the tag (name)
    std::string name;

    /// Description associated with the tag
    std::string description;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(PixelValueUnit);
    TABLE_DECLARE_MANDATORY_FUNCTIONS(PixelValueUnit);
    TABLE_DECLARE_OPTIONAL_FUNCTIONS(PixelValueUnit);
    // ------------------------------------------------------------------------

  protected:
    PixelValueUnit();

  }; // end of class

}
// --------------------------------------------------------------------

#endif