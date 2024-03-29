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
#include "sydFieldBase.h"

// odb
#include <odb/sqlite/database.hxx>
#include <odb/schema-catalog.hxx>

// --------------------------------------------------------------------
namespace syd {

  // The following classes will be defined elsewhere
  template<class DatabaseSchema> class DatabaseCreator;

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

    /// Destructor
    virtual ~Database();

    /// Type of a generic record (pointer)
    typedef typename std::shared_ptr<syd::Record> RecordBasePointer;

    /// Type of a generic vector of records (pointer)
    typedef std::vector<RecordBasePointer> RecordBaseVector;

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
    RecordBasePointer New(const std::string & table_name);

    // Create a new record of the specified table type (RecordType)
    template<class RecordType> typename RecordType::pointer New();
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Type of the map that contains the association between names and tables
    typedef std::map<std::string, syd::RecordTraitsBase*, case_insensitive_comp> MapOfTraitsType;

    /// Return the map that contains the association between names and tables
    const MapOfTraitsType & GetTraitsMap() const { return map_of_traits_; }
    syd::RecordTraitsBase * GetTraits(const std::string & table_name) const;
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// All Query function allocate new records
    template<class RecordType> typename RecordType::pointer
      QueryOne(const odb::query<RecordType> & q) const;
    template<class RecordType> void
      QueryOne(std::shared_ptr<RecordType> & p,
               const odb::query<RecordType> & q) const { p = QueryOne(q); }
    template<class RecordType> void
      QueryOne(std::shared_ptr<RecordType> & p,
               IdType id) const { p = QueryOne<RecordType>(id); }
    template<class RecordType> typename RecordType::pointer
      QueryOne(IdType id) const;
    RecordBasePointer
      QueryOne(const std::string & table_name, IdType id) const;
    template<class RecordType> void
      Query(std::vector<std::shared_ptr<RecordType>> & records,
            const odb::query<RecordType> & q) const;
    template<class RecordType> void
      Query(std::vector<std::shared_ptr<RecordType>> & records) const;
    template<class RecordType> void
      Query(std::vector<std::shared_ptr<RecordType>> & records,
            const std::vector<syd::IdType> & ids) const;
    void Query(RecordBaseVector & records,
               const std::string table_name,
               const std::vector<syd::IdType> & ids) const;
    void Query(RecordBaseVector & records, const std::string table_name) const;
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Insert
    void Insert(RecordBasePointer record);
    void Insert(RecordBaseVector record, const std::string & table_name);
    template<class RecordType>
      void Insert(std::shared_ptr<RecordType> record);
    template<class RecordType>
      void Insert(std::vector<std::shared_ptr<RecordType>> records);
    virtual void InsertDefaultRecords(const std::string & def) {}
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Update
    void Update(RecordBasePointer record);
    void Update(RecordBaseVector record, const std::string & table_name);
    template<class RecordType>
      void Update(std::shared_ptr<RecordType> record);
    template<class RecordType>
      void Update(std::vector<std::shared_ptr<RecordType>> records);
    void UpdateField(RecordBasePointer & record,
                     std::string field_name,
                     std::string value_name);
    template<class RecordType>
      void UpdateField(std::shared_ptr<RecordType> & record,
                       std::string field_name,
                       std::string value_name);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    void Delete(RecordBaseVector & records, const std::string & table_name);
    void Delete(RecordBasePointer record);
    template<class RecordType>
      void Delete(std::shared_ptr<RecordType> record);
    template<class RecordType>
      void Delete(std::vector<std::shared_ptr<RecordType>> & records);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Find (grep)
    template<class RecordType>
      void Grep(std::vector<std::shared_ptr<RecordType>> & output,
                const std::vector<std::shared_ptr<RecordType>> & input,
                const std::vector<std::string> & patterns,
                const std::vector<std::string> & exclude);
    template<class RecordType>
      void GrepAllFields(std::vector<std::shared_ptr<RecordType>> & output,
                         const std::vector<std::shared_ptr<RecordType>> & input,
                         const std::vector<std::string> & patterns,
                         const std::vector<std::string> & exclude);
    bool Grep(const std::string & s,
              const std::vector<std::string> & patterns,
              const std::vector<std::string> & exclude);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Generic dump, display the list of tables
    virtual void Dump(std::ostream & os = std::cout);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Sort records (to be specialized in traits>Sort)
    template<class RecordType>
      void Sort(std::vector<std::shared_ptr<RecordType>> & records,
                const std::string & type="") const;
    /// Sort generic records
    virtual void Sort(RecordBaseVector & records,
                      const std::string & table_name,
                      const std::string & type="") const;
    // ------------------------------------------------------------------------


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
    /// Call back when busy
    int BusyHandlerCallback(int retry);


    // ------------------------------------------------------------------------
    /// Return a string with the list of the table names
    std::string GetListOfTableNames() const;

    /// Get the number of elements in the table
    long GetNumberOfElements(const std::string & table_name) const;

    /// Get the number of elements in the table, knowing the type
    //template<class RecordType> long GetNumberOfElements() const;

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

    /// Build the given Field of the given table_name
    syd::FieldBase::pointer GetField(std::string table_name, std::string field_name, std::string abbrev="") const;

    /// Build the given Field of the given table_name
    syd::FieldBase::vector GetFields(std::string table_name, std::string field_names) const;

    // ----------------------------------------------------------------------------------
  protected:
    // Create an empty database
    Database();

    /// Main function to open a database
    void OpenFromFile(std::string filename);

    /// Must be overwritten by concrete classes.
    virtual void CreateTables() = 0;

    /// Declare a new table in the database
    template<class Record> void AddTable();

    /// Delete files when needed
    void DeleteFiles();
    std::vector<std::string> files_to_delete_;

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
