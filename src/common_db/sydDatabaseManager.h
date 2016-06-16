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

#ifndef SYDDATABASEMANAGER_H
#define SYDDATABASEMANAGER_H

// syd
#include "sydDatabase.h"
#include "sydDatabaseCreator.h"

// --------------------------------------------------------------------
namespace syd {

  //--------------------------------------------------------------------
  /// Manage a set of database schema. Allow to read and create.
  class DatabaseManager {
  public:

    /// Retrieve the unique instance (singleton)
    static DatabaseManager * GetInstance();

    /// Open and read a database (automatically find the correct schema)
    Database * Open(std::string filename);

    /// Open and read a database of the given schema
    template<class DatabaseSchema>
    DatabaseSchema * Open(const std::string & filename);

    /// Create a new empty database of the given db_schema_name
    void Create(const std::string & db_schema_name, const std::string & filename,
                const std::string & folder, bool force_overwrite=false);

    /// Return a list of registered database schema names
    const std::vector<std::string> & GetDatabaseSchemas() const { return db_schema_names_; }

    /// To register a new database schema -> for plugin only
    template<class DatabaseSchema>
    DatabaseCreator<DatabaseSchema> * RegisterDatabaseSchema(const std::string & type);

    /// Store a list of all loaded database (to be able to retrive the db from a record)
    static std::map<odb::database *, syd::Database *> & GetListOfLoadedDatabases();

  protected:
    /// Purposely protected, only a single instance possible
    DatabaseManager() {}
    ~DatabaseManager() { }

    /// List of map between db types and db creators
    std::map<std::string, DatabaseCreatorBase*> db_map_;

    /// List of registered db types
    std::vector<std::string> db_schema_names_;

  };

  // This macro must be used in the .cxx file of a new type of
  // database to register it to the list of known db type. When used
  // as a plugin a mecanism is used to prevent registering two times
  // the class.
#define SYD_REGISTER_DATABASE(classname, name)                          \
  static syd::DatabaseCreator<classname> * name ## _static_registration \
  = syd::DatabaseManager::GetInstance()->RegisterDatabaseSchema<classname>(#name);

#include "sydDatabaseManager.txx"

} // end namespace

// --------------------------------------------------------------------

#endif
