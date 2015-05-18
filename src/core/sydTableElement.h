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

#ifndef SYDELEMENT_H
#define SYDELEMENT_H

// syd
#include "sydCommon.h"

// --------------------------------------------------------------------
namespace syd {

  /// Base class for all element (row) in a table
  class TableElement {
  public:

    /// Default function to print an element (must be inline here).
    friend std::ostream& operator<<(std::ostream& os, const TableElement & p) {
      os << p.ToString();
      return os;
    }

    /// Use to write the element as a string (must be overloaded)
    virtual std::string ToString() const;

    /// Set basics values from a list of string
    virtual void Set(std::vector<std::string> & arg);

  }; // end of class

#define SET_TABLE_NAME(name) static std::string GetTableName() { return name; }

} // end namespace
// --------------------------------------------------------------------

#endif
