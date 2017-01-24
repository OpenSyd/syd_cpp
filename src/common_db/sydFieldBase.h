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

#ifndef SYDFIELDBASE_H
#define SYDFIELDBASE_H

// syd
#include "sydCommon.h"

// --------------------------------------------------------------------
namespace syd {

  class Record;
  class Database;

  /*
    This class is the base class for all fields
  */
  class FieldBase {
  public:

    typedef std::shared_ptr<FieldBase> pointer;
    typedef std::vector<pointer> vector;
    typedef std::shared_ptr<Record> RecordPointer;
    typedef std::function<std::string (RecordPointer p)> GenericFunction;

    FieldBase(std::string name);  // FIXME ? protected
    virtual ~FieldBase();

    std::string name;
    std::string type;
    int precision;
    GenericFunction gf; // protected ?

    /// Get value
    std::string get(RecordPointer p);

    /// Set value
    // FIXME    virtual void set(syd::Record::pointer p, std::string value); //later

    /// Create the generic function 'f'
    virtual pointer CreateField(const syd::Database * db, std::string field_names) const = 0;

  }; // end of class

  // --------------------------------------------------------------------
} // end namespace

// --------------------------------------------------------------------
#endif
