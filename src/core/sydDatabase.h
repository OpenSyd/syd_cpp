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
#include "sydTableBase.h"
#include "core/sydDatabaseInformation-odb.hxx"

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

    // ------------------------------------------------------------------------
    /// Return the type of the db (read in the file)
    std::string GetDatabaseSchema() { return database_schema_; }

    /// Return the filename (.db file sqlite)
    std::string GetFilename() const { return filename_; }

    /// Return the folder that contains the associated images (relative to the current path)
    std::string GetDatabaseRelativeFolder() const { return relative_folder_; }

    /// Return the folder that contains the associated images (absolute)
    std::string GetDatabaseAbsoluteFolder() const { return absolute_folder_; }
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Insert a new element of type TableElement in the database
    template<class TableElement>
    void Insert(TableElement & r);

    /// Insert several elements at a time
    template<class TableElement>
    void Insert(std::vector<TableElement*>& r);

    /// Insert a new element build from set of string
    virtual TableElementBase * InsertFromArg(const std::string & table_name,
                                             std::vector<std::string> & arg);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Perform a query on the database and return the found elements in the list
    template<class TableElement>
    void Query(const odb::query<TableElement> & q, std::vector<TableElement> & list);

    /// Retrieve all elements of the table
    template<class TableElement>
    void Query(std::vector<TableElement> & list);

    /// Get the unique element matching the query (fail if 0 or >1 elements)
    template<class TableElement>
    TableElement QueryOne(const odb::query<TableElement> & q);

    /// Get the unique element with the given id
    template<class TableElement>
    TableElement QueryOne(IdType id);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Count the number of elements matching the query
    template<class TableElement>
    unsigned int Count(const odb::query<TableElement> & q);

    /// Check if the given id exist
    template<class TableElement>
    bool IfExist(IdType id);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Update an element in the database
    template<class TableElement>
    void Update(TableElement & r);

    /// Update a set of elements in the database
    template<class TableElement>
    void Update(std::vector<TableElement*> & r);
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    /// Call back for SQL query to the DB. For debug purpose only
    void TraceCallback(const char* sql);

    /// Return the last SQL query (set by TraceCallback). For debug purpose only
    std::string GetLastSQLQuery() const { return current_sql_query_; }
    // ------------------------------------------------------------------------


    // ------------------------------------------------------------------------
    void SetDeleteDryRunFlag(bool b) { delete_dry_run_flag_ = b; }

    /// Delete a single record
    template<class TableElement> void Delete(IdType id);

    /// Delete a single record
    template<class TableElement> void Delete(TableElement & e);

    /// Delete a single record
    virtual void Delete(const std::string & table_name, syd::IdType id);

    /// Delete several records
    template<class TableElement> int Delete(std::vector<TableElement> & ve);

    /// Delete several records
    template<class TableElement> int Delete(std::vector<IdType> & ids);

    /// Delete several records
    virtual void Delete(const std::string & table_name, std::vector<syd::IdType> & ids);

    /// Delete all records
    virtual void DeleteAll(const std::string & table_name);

    /// Callback called when a record is deleted. Do nothing here, be could be overloaded.
    virtual void OnDelete(const std::string & table_name, TableElementBase * elem);

    /// Add an element to the list of element to be delete
    template<class TableElement>
    void AddToDeleteList(TableElement & elem);

    /// Effectively delete current list of elements to be deleted
    virtual void DeleteCurrentList();
    // ------------------------------------------------------------------------



    /// -------------------------------- OLD

    //FIXME
    void CopyDatabaseTo(std::string file, std::string folder);
    template<class TableElement>
    bool TableIsEqual(syd::Database * db1);


    /// Delete the element with id in the given table
    // bool Delete(const std::string & table_name, const IdType id);

    /// Generic dump according to args
    virtual void Dump(const std::vector<std::string> & args, std::ostream & os);

    /// Dump all elements of a given table
    virtual void DumpTable(const std::string & table_name, std::ostream & os);

    /// Dump all elements of a given table (query[0])
    //    void DumpTable(const std::vector<std::string> & query, std::ostream & os);

    /// Return the (base) table with table_name
    TableBase * GetTable(const std::string & table_name);

    /// Return the table that contains TableElement
    template<class TableElement>
    Table<TableElement> * GetTable();

    std::string GetListOfTableNames();

    // ----------------------------------------------------------------------------------
    protected:
    // Create an empty database
    Database();

    /// Main function to open a database
    void Read(std::string filename);

    /// Must be overwritten by concrete classes.
    virtual void CreateTables() = 0;

    /// Add a table type managed by this class
    template<class TableElement>
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
    odb::sqlite::database * db_;

    /// Filename of the db
    std::string filename_;

    /// Main folder that contains images (read in the file)
    std::string relative_folder_;

    /// Main folder that contains images (read in the file)
    std::string absolute_folder_;

    /// Type of the database (read in the file)
    std::string database_schema_;

    /// Store current sql query for debug purpose
    std::string current_sql_query_;

    /// Store the list of elements (with their types) that will be deleted
    //    std::vector<std::pair<std::string, TableElementBase*>> list_of_elements_to_delete_;

    typedef std::pair<std::string, TableElementBase*> ElementPair;
    std::map<std::string, ElementPair> list_of_elements_to_delete_;

    /// Flag to not delete
    bool delete_dry_run_flag_;

  };

#include "sydDatabase.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
