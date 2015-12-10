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
#include "sydException.h"
#include "sydDatabaseInformation-odb.hxx"
#include "sydRecord-odb.hxx"
#include "sydRecordHistory-odb.hxx"
#include "sydRecordWithHistory-odb.hxx"
#include "sydTableBase.h"

// odb
#include <odb/sqlite/database.hxx>
#include <odb/sqlite/tracer.hxx>
#include <odb/sqlite/statement.hxx>
#include <odb/schema-catalog.hxx>

// --------------------------------------------------------------------
namespace syd {

  // The following classes will be defined elsewhere
  template<class Record> class Table;
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

    virtual ~Database();

    /// Type of a generic record (pointer)
    typedef syd::Record::pointer generic_record_pointer;

    /// Type of a generic vector of records (pointer)
    typedef syd::Record::vector generic_record_vector;

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


    /// Consider a relative path check if exist, create if not.
    void CheckOrCreateRelativePath(std::string relative_path);

    /* In the following using:
       std::shared_ptr<RecordType> and std::vector<std::shared_ptr<RecordType>>
       instead of typename RecordType::pointer or RecordType::vector
       is done purposely because it allows the compiler to detect the template
       without explicit specification:
       db->Query(patients);
       instead of
       db->Query<syd::Patient>(patients);
    */

    // ------------------------------------------------------------------------
    /// Create a new record of the specified table.
    std::shared_ptr<Record> New(const std::string & table_name) const;

    /// Create a new record of the table given by RecordType
    template<class RecordType>
    void New(std::shared_ptr<RecordType> & record) const;
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Insert an element. The type of the element is unknown
    void Insert(generic_record_pointer record);

    /// Insert several elements. The type of the element is unknown
    void Insert(generic_record_vector record, const std::string & table_name);

    /// Insert an element
    template<class RecordType>
    void Insert(std::shared_ptr<RecordType> record);

    /// Insert several elements
    template<class RecordType>
    void Insert(std::vector<std::shared_ptr<RecordType>> records);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Update an element. The type of the element is unknown
    void Update(generic_record_pointer record);

    /// Update several elements. The type of the element is unknown
    void Update(generic_record_vector record, const std::string & table_name);

    /// Update an element
    template<class RecordType>
    void Update(std::shared_ptr<RecordType> record);

    /// Update several elements
    template<class RecordType>
    void Update(std::vector<std::shared_ptr<RecordType>> records);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Set parameter of an element. 'Set' must be overwritten in the Record.
    virtual void Set(generic_record_pointer record, const std::vector<std::string> & args) const;
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// All Query function allocate new records

    /// Query a single record according to query
    template<class RecordType>
    void QueryOne(std::shared_ptr<RecordType> & record, const odb::query<RecordType> & q) const;

    /// Query a single record according to the id
    template<class RecordType>
    void QueryOne(std::shared_ptr<RecordType> & record, const IdType & id) const;

    /// Query a single record from the table_name
    void QueryOne(generic_record_pointer & r, const std::string & table_name, const IdType & id) const;

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
    void Query(generic_record_vector & records, const std::string table_name, const std::vector<syd::IdType> & ids) const;

    /// Query all records of the given tables
    void Query(generic_record_vector & records, const std::string table_name) const;

    /// Find (grep)
    template<class RecordType>
    void Grep(std::vector<std::shared_ptr<RecordType>> & output,
              const std::vector<std::shared_ptr<RecordType>> & input,
              const std::vector<std::string> & patterns,
              const std::vector<std::string> & exclude);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Generic dump, display the list of tables
    virtual void Dump(std::ostream & os = std::cout) const;

    /// Display all elements of a given tables
    void Dump(const std::string & table_name, const std::string & format="", std::ostream & os=std::cout) const;

    /// Dump the given records (according to a format). (Templated
    /// needed because dont know how to substitute vector<ppatient>
    /// with vector<precord>)
    template<class RecordType>
    void Dump(const std::vector<std::shared_ptr<RecordType>> & records,
              const std::string & format="",
              std::ostream & os=std::cout) const;
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Sort records (to be specialized in record->Sort
    template<class RecordType>
    void Sort(std::vector<std::shared_ptr<RecordType>> & records, const std::string & type="") const;
    /// Sort generic records
    virtual void Sort(generic_record_vector & records,
                      const std::string & table_name,
                      const std::string & type="") const;
    // ------------------------------------------------------------------------



    // ------------------------------------------------------------------------
    void Delete(generic_record_vector & records, const std::string & table_name);
    template<class RecordType>
    void Delete(std::shared_ptr<RecordType> record);
    template<class RecordType>
    void Delete(std::vector<std::shared_ptr<RecordType>> & records);
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
    template<class RecordType>
    Table<RecordType> * GetTable() const;

    /// Return a string with the list of the table names
    std::string GetListOfTableNames() const;

    /// Get the number of elements in the table
    long GetNumberOfElements(const std::string & table_name) const;

    /// Get the number of elements in the table, knowing the type
    template<class RecordType>
    long GetNumberOfElements() const;

    static std::map<odb::database *, syd::Database *> ListOfLoadedDatabases;

    // FIXME
    odb::sqlite::database * GetODB_DB() const { return odb_db_; }

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
    MapOfTablesType map_;

    /// Copy of the map with the table name in lowercase (for comparison)
    MapOfTablesType map_lowercase_;

    /// Return the map that contains the association between names and tables
    MapOfTablesType & GetMapOfTables() { return map_; }

    /// The sqlite database
    //std::unique_ptr<odb::sqlite::database> db_;
    odb::sqlite::database * odb_db_;

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
