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

#ifndef SYDRECORD_H
#define SYDRECORD_H

// syd
#include "sydCommon.h"
#include <odb/core.hxx>

// --------------------------------------------------------------------
namespace syd {

  /// Base class for all record (or element, or row) in a table
#pragma db object abstract
  class Record {
  public:

    virtual ~Record() { DD("dest Record"); }

#pragma db id auto
    /// Main key (automated, unique)
    IdType id;

    /// Default function to print an element (must be inline here).
    friend std::ostream& operator<<(std::ostream& os, const Record & p) {
      os << p.ToString();
      return os;
    }

    /// Default function to print a pointer to an element (must be inline here).
    template<class R>
    friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<R> p) {
      os << p->ToString();
      return os;
    }

    /// Use to write the element as a string (must be overloaded)
    virtual std::string ToString() const = 0;

   protected:
    Record(std::string) {}


  }; // end of class

  //#define SET_TABLE_NAME(name) static std::string GetTableName() { return name; }

} // end namespace
  // --------------------------------------------------------------------

#endif
