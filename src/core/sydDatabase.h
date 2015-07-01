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

#ifndef SYDDATABASE_H
#define SYDDATABASE_H

// syd
#include "sydCommon.h"
#include "sydException.h"
#include "sydDatabaseInformation-odb.hxx"
#include "sydRecord-odb.hxx"
#include "sydTableBase.h"

// odb
#include <odb/sqlite/database.hxx>
#include <odb/sqlite/tracer.hxx>
#include <odb/sqlite/statement.hxx>
#include <odb/schema-catalog.hxx>

// --------------------------------------------------------------------
namespace syd {

  // The following classes will be defined elsewhere
  template<class TableElementBase> class Table;
  template<class DatabaseSchema> class DatabaseCreator;

  /// This is the base class of all databases. Manage a sqlite
  /// database and an associated folder (containing images). Provide
  /// convenient functions to perform basic queries. The database is
  /// composed of a list of tables. Use DatabaseManager to Create or
  /// Read a database.
  class Database {

    // Only the class DatabaseCreator can acces to protected members such as Open and Create.
    template<class DatabaseSchema> friend class DatabaseCreator;

  public:

    typedef syd::Record::pointer record_pointer;
    typedef syd::Record::vector record_vector;

    // ------------------------------------------------------------------------
    /// Return the type of the db (read in the file)
    std::string GetDatabaseSchema() const { return database_schema_; }

    /// Return the filename (.db file sqlite)
    std::string GetFilename() const { return filename_; }

    /// Return the folder that contains the associated images (relative to the current path)
    std::string GetDatabaseRelativeFolder() const { return relative_folder_; }

    /// Return the folder that contains the associated images (absolute)
   std::string GetDatabaseAbsoluteFolder() const { return absolute_folder_; }

    /// Return absolute path from a relative one
    std::string ConvertToAbsolutePath(std::string relative_path) const { return absolute_folder_+PATH_SEPARATOR+relative_path; }
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    virtual void Dump(std::ostream & os = std::cout) const;
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Create a new record of the specified table.
    std::shared_ptr<Record> New(const std::string & table_name) const;

    /// Create a new record knowing the type
    template<class RecordType>
    void New(std::shared_ptr<RecordType> & record) const;

    /// Create a new record knowing the type
    template<class RecordType>
    std::shared_ptr<RecordType> New() const;
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Insert an element. The type of the element is
    void Insert(record_pointer record);

    /// Insert an element
    template<class RecordType>
    void Insert(std::shared_ptr<RecordType> record);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Set parameter of an element. 'Set' must be overwritten in the Record.
    virtual void Set(record_pointer record, const std::vector<std::string> & args) const;
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    //    virtual record_pointer Find(const std::string & table_name, IdType id) const;
    /// Query a single record according to query
    template<class RecordType>
    void QueryOne(std::shared_ptr<RecordType> & record, const odb::query<RecordType> & q) const;

    /// Query a single record according to the id
    template<class RecordType>
    void QueryOne(std::shared_ptr<RecordType> & record, const IdType & id) const;

    /// Query a single record from the table_name
    void QueryOne(record_pointer & r, const std::string & table_name, const IdType & id) const;

    /// Query several records according to query
    template<class RecordType>
    void Query(std::vector<std::shared_ptr<RecordType>> & records, const odb::query<RecordType> & q) const;

    /// Query all records
    template<class RecordType>
    void Query(std::vector<std::shared_ptr<RecordType>> & records) const;

    /// Query several records according to their id
    template<class RecordType>
    void Query(std::vector<std::shared_ptr<RecordType>> & records, const std::vector<syd::IdType> & ids) const;

    /// Query several records according to their id
    void Query(std::vector<record_pointer> & records, const std::string table_name, const std::vector<syd::IdType> & ids) const;
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Call back for SQL query to the DB. For debug purpose only
    void TraceCallback(const char* sql);

    /// Return the last SQL query (set by TraceCallback). For debug purpose only
    std::string GetLastSQLQuery() const { return current_sql_query_; }
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Return the (base) table with table_name
    TableBase * GetTable(const std::string & table_name) const;

    std::string GetListOfTableNames() const;

    // ----------------------------------------------------------------------------------
    protected:
    // Create an empty database
    Database();

    /// Main function to open a database
    void Read(std::string filename);

    /// Must be overwritten by concrete classes.
    virtual void CreateTables() = 0;

    /// Declare a new table in the database
    template<class Record>
    void AddTable();

    /// Type of the map that contains the association between names and tables
    typedef std::map<std::string, TableBase*> MapOfTablesType;

    /// Map that contains the association between names and tables
    MapOfTablesType map;

    /// Copy of the map with the table name in lowercase (for comparison)
    MapOfTablesType map_lowercase;

    /// Return the map that contains the association between names and tables
    MapOfTablesType & GetMapOfTables() { return map; }

    /// The sqlite database
    //std::unique_ptr<odb::sqlite::database> db_;
    odb::sqlite::database * db_;

    /// Filename of the db
    std::string filename_;

    /// Main folder that contains images (read in the file)
    std::string relative_folder_;

    /// Main folder that contains images (computed according to initial filename)
    std::string absolute_folder_;

    /// Type of the database (read in the file)
    std::string database_schema_;

    /// Store current sql query for debug purpose
    std::string current_sql_query_;

  };

#include "sydDatabase.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
