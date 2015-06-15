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

    /// Simple constructor, store the pointer to the db and the name of the table
    Table(syd::Database * db, odb::sqlite::database * d);

    static std::string GetTableName() { return TableElement::GetTableName(); }

    /// Compute the number of elements in this table
    virtual unsigned int GetNumberOfElements();

    /// Add the record with the given id to the list of records to be deleted
    virtual void AddToDeleteList(syd::IdType id);

    /// Add the records with the given ids to the list of records to be deleted
    virtual void AddToDeleteList(std::vector<syd::IdType> & ids);

    /// Add all records to the list of records to be deleted
    virtual void AddAllToDeleteList();

    /// Dump the content of the table (simple output)
    virtual void DumpTable(std::ostream & os);

    /// Perform a query on the database and return the found elements in the list
    void Query(const odb::query<TableElement> & q, std::vector<TableElement> & list);

    /// Get the list of element from the list of ids
    void Query(const std::vector<IdType> & ids, std::vector<TableElement> & list);

    /// Retrieve all elements of the table
    void Query(std::vector<TableElement> & list);

    /// Get the unique element matching the query (fail if 0 or >1 elements)
    TableElement QueryOne(const odb::query<TableElement> & q);

    /// Get the unique element with the given id
    TableElement QueryOne(IdType id);

    /// Count the number of elements matching the query
    unsigned int Count(const odb::query<TableElement> & q);

    /// Check if the given id exist
    bool IfExist(IdType id);

    /// Insert a new element in the database
    void Insert(TableElement & r);

    /// Insert a set of elements in the database
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

  protected:
    syd::Database * database_;

  };

#include "sydTable.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
