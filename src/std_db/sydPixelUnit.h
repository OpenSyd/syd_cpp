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

#ifndef SYDPIXELUNIT_H
#define SYDPIXELUNIT_H

// syd
#include "sydRecord.h"
#include "sydPrintTable.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::PixelUnit")
  /// Simple table to store a label and a description
  class PixelUnit : public syd::Record {
  public:

    DEFINE_TABLE_CLASS(PixelUnit);

#pragma db options("UNIQUE")
    /// Label of the tag (name)
    std::string name;

    /// Description associated with the tag
    std::string description;

    /// Write the element as a string
    virtual std::string ToString() const;

    void Set(const std::vector<std::string> & arg);

    virtual void DumpInTable(syd::PrintTable & table) const;

  protected:
    PixelUnit();

  }; // end of class

}
// --------------------------------------------------------------------

#endif
