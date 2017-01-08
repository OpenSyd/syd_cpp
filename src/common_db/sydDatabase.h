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
  ===========================================================================**/

#ifndef SYDDATABASE_H
#define SYDDATABASE_H

// syd
#include "sydException.h"
#include "sydRecord.h"
#include "sydDatabaseInformation-odb.hxx"
#include "sydRecord-odb.hxx"
#include "sydFile-odb.hxx"
#include "sydRecordHistory-odb.hxx"
#include "sydRecordWithHistory-odb.hxx"
#include "sydTag-odb.hxx"
#include "sydRecordWithTags-odb.hxx"
#include "sydTableBase.h" // FIXME 
//#include "sydDatabaseDescription.h"

// odb
#include <odb/sqlite/database.hxx>
#include <odb/schema-catalog.hxx>

// --------------------------------------------------------------------
namespace syd {

  // The following classes will be defined elsewhere
  template<class Record> class Table; // FIXME TO REMOVE
  template<class DatabaseSchema> class DatabaseCreator;
  class RecordTraitsBase;

  // Comparator function for case insensitive map
  /// (http://stackoverflow.com/questions/19102195/how-to-make-stlmap-key-case-insensitive)
  struct case_insensitive_comp {
    bool operator() (const std::string& lhs, const std::string& rhs) const {
      return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }
  };

  // I dont know how to retrieve this value, so I fix it here. It is
  // used to split large query, in particular the ones with with
  // "in_range".
  const unsigned int SQLITE_MAX_VARIABLE_NUMBER = 999;

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
    typedef typename std::shared_ptr<syd::Record> generic_record_pointer;

    /// Type of a generic vector of records (pointer)
    typedef std::vector<generic_record_pointer> generic_record_vector;

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
    std::string ConvertToAbsolutePath(std::string relative_path) const;

    /// Return a unique absolute path of a temporary filename inside the db
    std::string GetUniqueTempFilename(std::string extension=".mhd") const;

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
    generic_record_pointer New(const std::string & table_name);

    // Create a new record of the specified table type (RecordType)
    template<class RecordType>
      typename RecordType::pointer New();
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Type of the map that contains the association between names and tables
    typedef std::map<std::string, TableBase*> MapOfTablesType;
    typedef std::map<std::string, syd::RecordTraitsBase*, case_insensitive_comp> MapOfTraitsType;

    /// Return the map that contains the association between names and tables
    // FIXME
    const MapOfTablesType & GetMapOfTables() const { return map_; }
    const MapOfTraitsType & GetTraitsMap() const { return map_of_traits_; }
    syd::RecordTraitsBase * GetTraits(const std::string & table_name) const;
    // ------------------------------------------------------------------------



    // ------------------------------------------------------------------------
    /// All Query function allocate new records
    template<class RecordType>
      typename RecordType::pointer
      QueryOne(const odb::query<RecordType> & q) const;
    template<class RecordType>
      void QueryOne(std::shared_ptr<RecordType> & p,
                    const odb::query<RecordType> & q) const { p = QueryOne(q); }
    template<class RecordType>
      void QueryOne(std::shared_ptr<RecordType> & p,
                    IdType id) const { p = QueryOne<RecordType>(id); }
    template<class RecordType>
      typename RecordType::pointer
      QueryOne(IdType id) const;
    generic_record_pointer
      QueryOne(const std::string & table_name, IdType id) const;
    template<class RecordType>
      void Query(std::vector<std::shared_ptr<RecordType>> & records,
                 const odb::query<RecordType> & q) const;
    template<class RecordType>
      void Query(std::vector<std::shared_ptr<RecordType>> & records) const;
    template<class RecordType>
      void Query(std::vector<std::shared_ptr<RecordType>> & records,
                 const std::vector<syd::IdType> & ids) const;
    void Query(generic_record_vector & records,
               const std::string table_name,
               const std::vector<syd::IdType> & ids) const;
    void Query(generic_record_vector & records, const std::string table_name) const;
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Insert
    void Insert(generic_record_pointer record);
    void Insert(generic_record_vector record, const std::string & table_name);
    template<class RecordType>
      void Insert(std::shared_ptr<RecordType> record);
    template<class RecordType>
      void Insert(std::vector<std::shared_ptr<RecordType>> records);
    virtual void InsertDefaultRecords(const std::string & def) {}
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Update
    void Update(generic_record_pointer record);
    void Update(generic_record_vector record, const std::string & table_name);
    template<class RecordType>
      void Update(std::shared_ptr<RecordType> record);
    template<class RecordType>
      void Update(std::vector<std::shared_ptr<RecordType>> records);
    // FIXME 
    void Update(generic_record_pointer & record, std::string field_name,
                std::string value_name);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    void Delete(generic_record_vector & records, const std::string & table_name);
    void Delete(generic_record_pointer record);
    template<class RecordType>
      void Delete(std::shared_ptr<RecordType> record);
    template<class RecordType>
      void Delete(std::vector<std::shared_ptr<RecordType>> & records);
    // ------------------------------------------------------------------------



    /// Find (grep)
    template<class RecordType>
      void Grep(std::vector<std::shared_ptr<RecordType>> & output,
                const std::vector<std::shared_ptr<RecordType>> & input,
                const std::vector<std::string> & patterns,
                const std::vector<std::string> & exclude);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Generic dump, display the list of tables
    virtual void Dump(std::ostream & os = std::cout);

    /// Display all elements of a given tables
    void Dump(const std::string & table_name,
              const std::string & format="",
              std::ostream & os=std::cout);

    /// Dump the given records (according to a format). (Templated
    /// needed because dont know how to substitute vector<ppatient>
    /// with vector<precord>)
    template<class RecordType>
      void Dump(const std::vector<std::shared_ptr<RecordType>> & records,
                const std::string & format="",
                std::ostream & os=std::cout);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Sort records (to be specialized in record->Sort
    template<class RecordType>
      void Sort(std::vector<std::shared_ptr<RecordType>> & records,
                const std::string & type="") const;
    /// Sort generic records
    virtual void Sort(generic_record_vector & records,
                      const std::string & table_name,
                      const std::string & type="") const;
    // ------------------------------------------------------------------------



    /// Add a file to the files to delete (used by sydFile)
    void AddFilenameToDelete(const std::string & f);
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

    /// Return (compute the first time) the db SQL description
    //    syd::DatabaseDescription * GetDatabaseDescription();

    /// Return the sql descriptio of the table
    //    syd::TableDescription * GetTableDescription(const std::string & table_name);

    /// Get the number of elements in the table (cannot be const
    /// because create db description)
    long GetNumberOfElements(const std::string & table_name);

    /// Get the number of elements in the table, knowing the type
    template<class RecordType>
      long GetNumberOfElements();

    // FIXME
    odb::sqlite::database * GetODB_DB() const { return odb_db_; }
    sqlite3 * GetSqliteHandle() const;

    // FIXME to remove ? (never used)
    void CheckDatabaseSchema();

    /// Allow to migrate the schema when the db version in the file is
    /// different from the syd version.
    void MigrateSchema();

    /// Copy only the db file (warning not the folder)
    void Copy(std::string dbname);

    /// Copy everything the db file + all files in folder (could be long!)
    void Copy(std::string dbname, std::string folder);

    /// Global flag
    void SetOverwriteFileFlag(bool b) { overwrite_file_if_exists_flag_ = b; }
    bool GetOverwriteFileFlag() const { return overwrite_file_if_exists_flag_; }

    /// Get a function that retrive a field value (string) according to table_name and field name
    //    const syd::Record::GetFieldFunction & FieldGetter(std::string table_name, std::string field_name) const;

    /*    static const syd::Record::GetFieldFunction & GetFieldFunction(std::string table_name,
          std::string field_name) const;
    */

    /// Get (initialize) the map of field getter function
    //    const std::map<std::string, std::map<std::string, syd::Record::GetFieldFunction>> & GetDefaultFields() const;

    // ----------------------------------------------------------------------------------
  protected:
    // Create an empty database
    Database();

    /// Main function to open a database
    void OpenFromFile(std::string filename);

    /// Must be overwritten by concrete classes.
    virtual void CreateTables() = 0;

    /// Declare a new table in the database
    template<class Record>
      void AddTable();

    /// Delete files when needed
    void DeleteFiles();

    std::vector<std::string> files_to_delete_;

    /// Map that contains the association between names and tables
    MapOfTablesType map_;

    /// Copy of the map with the table name in lowercase (for comparison)
    MapOfTablesType map_lowercase_;

    /// Map of traits
    MapOfTraitsType map_of_traits_;

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

    /// Store the OO db schema description
    //    syd::DatabaseDescription * description_;

    /// Global flag (will be used when write a file in the db)
    bool overwrite_file_if_exists_flag_;
  };


  // Helpers function to simplify native sqlite query
  std::string sqlite3_column_text_string(sqlite3_stmt * stmt, int iCol);

} // end namespace

// Outside namespace
#include "sydDatabase.txx"

// --------------------------------------------------------------------

#endif
