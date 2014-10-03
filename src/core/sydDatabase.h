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
#include "sydDatabaseFactory.h"

// odb
#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/sqlite/database.hxx>
#include <odb/sqlite/tracer.hxx>
#include <odb/sqlite/statement.hxx>
#include <odb/schema-catalog.hxx>

// dcmtk
#include "dcmtk/dcmdata/dctk.h"

// --------------------------------------------------------------------
namespace syd {

  class Database {
  public:

    // Main static function to create a new database
    static std::shared_ptr<Database> OpenDatabase(std::string name, std::string init_filename="");

    // This helper function allow to open a new database knowing his type (DatabaseType)
    template<class DatabaseType>
    static std::shared_ptr<DatabaseType> OpenDatabaseType(std::string name);

    // Functions common to all databases
    virtual void Dump(std::ostream & os, std::vector<std::string> & args) = 0;
    virtual void CheckIntegrity(std::vector<std::string> & args) = 0;
    virtual void CreateDatabase() { DD("TODO"); }

    // Accessors
    std::string get_name() const { return name_; }
    virtual std::string get_typename() const = 0;
    std::string get_folder() const { return folder_; }
    std::string get_filename() const { return filename_; }

    // Call back for SQL query to the DB. For debug purpose
    void TraceCallback(const char* sql);

    // Load the list of T corresponding to the sql query q
    template<class T> void LoadVector(std::vector<T> & list, const odb::query<T> & q= odb::query<T>::id != 0);

    // Insert a new element of type T
    template<class T> void Insert(T & t);

    // Update an element of type T
    template<class T> void Update(T & t);

    // Remove an element of type T
    template<class T> void Erase(T & t);
    template<class T> void Erase(std::vector<T> & t);

    // Retrieve the element with a given id
    template<class T> T GetById(IdType id);

    // Check if an element exist an if yes, retrieve it
    template<class T> bool GetIfExist(odb::query<T> q, T & t);

    // Get an element or insert a new one
    template<class T> bool GetOrInsert(odb::query<T> q, T & t);

  protected:
    Database(std::string name);
    virtual void OpenSqliteDatabase(std::string filename, std::string folder);

    // help function to read filename/folder from a set of string
    void SetFileAndFolder(std::istringstream & f);

    odb::sqlite::database * db_;
    std::string filename_;
    std::string folder_;
    std::string name_;
    std::string current_sql_query_;
  };



#include "sydDatabase.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
