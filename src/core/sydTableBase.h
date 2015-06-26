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
#include "sydCommon.h"
#include "sydTableElementBase.h"
#include "sydDatabase.h"

// odb
#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/sqlite/database.hxx>
#include <odb/sqlite/tracer.hxx>
#include <odb/sqlite/statement.hxx>
#include <odb/schema-catalog.hxx>

// --------------------------------------------------------------------
namespace syd {

  class Database;

  /// Base class for the functions that are common to all tables
  class TableBase {
  public:

    TableBase() {}
    // TableBase(syd::Database * db) { database_ = db; }

    // /// Constructor, must give the linked database.
    // TableBase(odb::sqlite::database * d):db_(d) {}

    virtual void Initialization() {};


    void SetSQLDatabase(odb::sqlite::database * d) { db_ = d; }
    void SetDatabase(syd::Database * db) { database_ = db; }

    /// Insert simple record from list of strings (pure virtual)
    virtual TableElementBase * InsertFromArg(std::vector<std::string> & arg) = 0;

    /// Dump all the element of the table
    virtual void Dump(std::ostream & os, const std::string & format) = 0;

    /// Dump all the given elements of the table
    virtual void Dump(std::ostream & os, const std::string & format, const std::vector<syd::IdType> & ids) = 0;

    // Retrieve the element's id that match the pattern (and dont match the exclude pattern)
    virtual void Find(std::vector<syd::IdType> & ids, const std::vector<std::string> & pattern, const std::vector<std::string> & exclude) = 0 ;

    /// Count the number of elements of a table. (to be changed)
    virtual unsigned int GetNumberOfElements() const = 0;

    /// Add the record with the given id to the list of records to be deleted
    virtual void AddToDeleteList(syd::IdType id) = 0;

    /// Add all records to the list of records to be deleted
    virtual void AddAllToDeleteList() = 0;

    /// Add the records with the given ids to the list of records to be deleted
    virtual void AddToDeleteList(std::vector<syd::IdType> & ids) = 0;

    /// Prepare to erase a record (without commit)
    virtual void Erase(TableElementBase * elem) = 0;

    odb::sqlite::database * GetSQLDatabase() { return db_; }

  protected:
    odb::sqlite::database * db_;
    syd::Database * database_;
  };

} // end namespace
// --------------------------------------------------------------------

#endif
