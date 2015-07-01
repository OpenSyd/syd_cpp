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

#ifndef SYDTABLE_H
#define SYDTABLE_H

// syd
//#include "sydTableBase.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  class Database;

  /// This class is used to encapsulate common functions for a
  /// database table that manage some Record.
  template<class RecordType>
  class Table: public TableBase {
  public:

    /// Generic record type
    typedef syd::Record::pointer record_pointer;
    typedef syd::Record::vector record_vector;

    /// Constructor, set the pointer to the database
    Table(syd::Database * d):TableBase() { db_ = d;}

    //// Default New, create a new record of the correct type, return a generic syd::Record
    virtual std::shared_ptr<syd::Record> New() const;

    /// Insert a new element in the table (knowing is type)
    virtual void Insert(record_pointer record) const;

    virtual void QueryOne(record_pointer & record, const syd::IdType & id) const;

    virtual void Query(record_vector & records, const std::vector<syd::IdType> & ids) const;

  protected:
    syd::Database * db_;

  };
} // end namespace


#include "sydTable.txx"


// --------------------------------------------------------------------

#endif
