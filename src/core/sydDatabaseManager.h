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

  /// Manage a set of database schema. Allow to read and create.
  class DatabaseManager {
  public:

    /// Retrieve the unique instance (singleton)
    static DatabaseManager * GetInstance();

    /// Open and read a database (automatically find the correct schema)
    Database * Read(const std::string & filename);

    /// Open and read a database of the given schema
    template<class DatabaseSchema>
    DatabaseSchema * Read(const std::string & filename);

    /// Create a new empty database of the given db_schema_name
    void Create(const std::string & db_schema_name, const std::string & filename, const std::string & folder);

    /// Return a list of registered database schema names
    const std::vector<std::string> & GetDatabaseSchemas() const { return db_schema_names_; }

    /// To register a new database schema -> for plugin only
    template<class DatabaseSchema>
    DatabaseCreator<DatabaseSchema> * RegisterDatabaseSchema(const std::string & type);

  protected:
    /// Purposely protected, only a single instance possible
    DatabaseManager() {}

    /// Unique instance (singleton). Because it is static, main must
    /// declare it only once, with : "syd::DatabaseManager *
    /// syd::DatabaseManager::singleton_;"
    static DatabaseManager * singleton_;

    /// List of map between db types and db creators
    std::map<std::string, DatabaseCreatorBase*> db_map_;

    /// List of registered db types
    std::vector<std::string> db_schema_names_;

  };

#include "sydDatabaseManager.txx"

} // end namespace

// --------------------------------------------------------------------

#endif
