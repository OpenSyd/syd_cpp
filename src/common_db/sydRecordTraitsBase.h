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

#ifndef SYDRECORDTRAITSBASE_H
#define SYDRECORDTRAITSBASE_H

// syd
#include "sydCommon.h"

// --------------------------------------------------------------------
namespace syd {

  /*
    This class containts common information (traits) for record.
    We store here information that do not depends on the RecordType.
  */

  class Record;
  class Database;
  class FieldBase;

  class RecordTraitsBase {
  public:

    typedef std::shared_ptr<Record> RecordBasePointer;
    typedef std::vector<RecordBasePointer> RecordBaseVector;
    typedef std::shared_ptr<FieldBase> FieldBasePointer;
    typedef std::vector<FieldBasePointer> FieldBaseVector;
    typedef std::map<std::string, std::string> FieldFormatMapType;
    typedef std::map<std::string, FieldBasePointer> FieldMapType;

    /// Return the table name
    virtual std::string GetTableName() const;
    virtual std::string GetSQLTableName() const;

    /// Create a new record
    virtual RecordBasePointer CreateNew(syd::Database * db) const = 0;

    /// Basic functions: Query Insert Update Delete
    virtual RecordBasePointer QueryOne(const syd::Database * db, syd::IdType id) const = 0;
    virtual void Query(const syd::Database * db,
                       RecordBaseVector & r,
                       const std::vector<syd::IdType> & id) const = 0;
    virtual void Query(const syd::Database * db, RecordBaseVector & r) const = 0;
    virtual void Insert(syd::Database * db, RecordBasePointer record) const = 0;
    virtual void Insert(syd::Database * db, const RecordBaseVector & records) const = 0;
    virtual void Update(syd::Database * db, RecordBasePointer record) const = 0;
    virtual void Update(syd::Database * db, const RecordBaseVector & records) const = 0;
    virtual void Delete(syd::Database * db, const RecordBaseVector & records) const = 0;

    /// Sort function
    virtual void Sort(RecordBaseVector & records, const std::string & type) const = 0;

    /// Return the map of all default fields
    virtual const FieldMapType & GetFieldsMap(const syd::Database * db) const = 0;

    /// Return the list of format type
    virtual const FieldFormatMapType & GetFieldFormatsMap(const syd::Database * db) const = 0;

    /// Get (copy) a field from name
    virtual FieldBasePointer GetField(const syd::Database * db,
                                      std::string field_name,
                                      std::string abbrev="") const = 0;

    /// Get (copy) fields from names separated with space
    virtual FieldBaseVector GetFields(const syd::Database * db,
                                      std::string field_name) const = 0;

  protected:
    RecordTraitsBase(std::string table_name);
    std::string table_name_;
    std::string sql_table_name_;

  }; // end of class
  // --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
