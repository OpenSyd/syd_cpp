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
#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to encapsulate common functions for a
  /// database table that manage some TableElement.
  template<class TableElement>
  class Table: public TableBase {
  public:

    Table():TableBase() {}
    //    Table(syd::Database * db):TableBase(db) {}

    /// Simple constructor, store the pointer to the db and the name of the table
    //    Table(syd::Database * db, odb::sqlite::database * d);

    static std::string GetTableName() { return TableElement::GetTableName(); }

    /// Compute the number of elements in this table
    virtual unsigned int GetNumberOfElements() const;

    /// Add the record with the given id to the list of records to be deleted
    virtual void AddToDeleteList(syd::IdType id);

    /// Add the records with the given ids to the list of records to be deleted
    virtual void AddToDeleteList(std::vector<syd::IdType> & ids);

    /// Add all records to the list of records to be deleted
    virtual void AddAllToDeleteList();

    /// Dump the content of all elements the table
    virtual void Dump(std::ostream & os, const std::string & format); // FIXME const

    /// Dump the content of all given elements the table
    virtual void Dump(std::ostream & os, const std::string & format, const std::vector<syd::IdType> & ids);

    /// Dump the content of all given elements the table
    virtual void Dump(std::ostream & os, const std::string & format, const std::vector<TableElement> & elements);

    /// Retrieve the element's id that match the pattern (minus the ones that match the exclude pattern)
    virtual void Find(std::vector<syd::IdType> & ids, const std::vector<std::string> & pattern, const std::vector<std::string> & exclude);

    /// Perform a query on the database and return the found elements in the list
    void Query(const odb::query<TableElement> & q, std::vector<TableElement> & list) const;

    /// Get the list of element from the list of ids
    void Query(const std::vector<IdType> & ids, std::vector<TableElement> & list) const;

    /// Retrieve all elements of the table
    void Query(std::vector<TableElement> & list) const;

    /// Get the unique element matching the query (fail if 0 or >1 elements)
    TableElement QueryOne(const odb::query<TableElement> & q) const;

    /// Get the unique element with the given id
    TableElement QueryOne(IdType id) const;

    /// Get the unique element with the given id
    void QueryOne(TableElement & e, IdType id) const;



    void TestQuery(std::shared_ptr<TableElement> & p) const;

    /// Count the number of elements matching the query
    unsigned int Count(const odb::query<TableElement> & q) const;

    /// Check if the given id exist
    bool IfExist(IdType id) const;

    /// Insert a new element in the database
    void Insert(TableElement & r);

    void TestInsert(std::shared_ptr<TableElement> r);

    /// Insert a set of elements in the database (with pointer to retrieve the correct id)
    void Insert(std::vector<TableElement*> & r);

    /// Update an element in the database
    void Update(TableElement & r);

    /// Update a set of elements in the database
    void Update(std::vector<TableElement*> & r);

    /// Update a set of elements in the database
    void Update(std::vector<TableElement> & r);

    /// default impltementation of insert, must be overloaded
    virtual TableElement * InsertFromArg(std::vector<std::string> & arg);

    /// Prepare to erase a record (without commit)
    virtual void Erase(TableElementBase * elem);

    /// Return the relative path of an element linked to a file. Must
    /// be specialized for all table.
    std::string GetRelativePath(const TableElement & e) const;

    /// Compute a (relative) folder that could contains element of the
    /// TableElement. Must be specialized for all table.
    std::string ComputeRelativeFolder(const TableElement & e);

    /// Set element properties from a set of strings
    virtual void Set(TableElement & e, const std::vector<std::string> & arg);

  // protected:
  //   syd::Database * database_;

  };

#include "sydTable.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
