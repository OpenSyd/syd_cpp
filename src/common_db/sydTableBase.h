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

#ifndef SYDTABLEBASE_H
#define SYDTABLEBASE_H

// syd
#include "sydDD.h"
#include "sydLog.h"
#include "sydDatabaseDescription.h"

// --------------------------------------------------------------------
namespace syd {

  //class Database;
  class Record;
  class Database;

  /// Base class for the functions that are common to all tables
  class TableBase {
  public:

    typedef syd::Record::pointer record_pointer;
    typedef syd::Record::vector record_vector;

    virtual record_pointer New() = 0;

    virtual void Insert(record_pointer record) const = 0;

    virtual void Insert(record_vector records) const = 0;

    virtual void Update(record_pointer record) const = 0;

    virtual void Update(record_vector records) const = 0;

    virtual void QueryOne(record_pointer & r, const syd::IdType & id) const = 0;

    virtual void Query(record_vector & r, const std::vector<syd::IdType> & id) const = 0;

    virtual void Query(record_vector & r) const = 0;

    virtual long GetNumberOfElements() const = 0;

    virtual void Sort(record_vector & records, const std::string & type) const = 0;

    virtual void Delete(record_vector & v) const = 0;

    /// Return the OO name of the table
    virtual std::string GetTableName() const = 0;

    /// Return the SQL name of the table
    virtual std::string GetSQLTableName() const = 0;

    /// Return the OO name of the inherit table
    virtual std::vector<std::string> & GetInheritSQLTableNames() const = 0;

    /// Return the table description
    //syd::TableDescription & GetTableDescription() { return table_description_; }

  protected:
    std::string inherit_table_name_;
  };

} // end namespace
// --------------------------------------------------------------------

#endif
