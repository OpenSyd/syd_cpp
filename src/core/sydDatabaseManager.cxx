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

// syd
#include "sydDatabaseManager.h"
#include "sydDatabaseSchemaName-odb.hxx"

// --------------------------------------------------------------------
syd::DatabaseManager * syd::DatabaseManager::GetInstance()
{
  if (singleton_ == NULL) {
    singleton_ = new DatabaseManager;
  }
  return singleton_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Database * syd::DatabaseManager::Read(const std::string & filename)
{
  odb::sqlite::database * db;
  try {
    LOG(5) << "Opening database '" << filename << "' to get typename.";
    db = new odb::sqlite::database(filename);
    odb::connection_ptr c(db->connection());
  }
  catch (const odb::exception& e) {
    LOG(FATAL) << "Cannot open db '" << filename << "' : " << e.what();
  }

  std::string db_schema_name;
  try {
    // Try to get the tabletype name
    odb::transaction transaction (db->begin());
    odb::query<syd::DatabaseInformation> q;
    typedef odb::result<syd::DatabaseInformation> result;
    result r(db->query< syd::DatabaseInformation >(q));
    syd::DatabaseInformation s;
    if (r.begin() == r.end()) {
      LOG(FATAL) << "Cannot find schema_version name in the db ?";
    }
    // We only consider the *last* element here. The table
    // schema_version can contains several schema names, but only the
    // last defined the database type.
    for(auto i=r.begin(); i != r.end(); i++) {
      r.begin().load(s);
    }
    transaction.commit();
    db_schema_name = s.database_schema;
    LOG(5) << "Database type is '" << db_schema_name << "'.";
  }
  catch (const odb::exception& e) {
    LOG(FATAL) << "Cannot find the db typename in '" << filename << "'."
               << std::endl << "A table name 'schema_version' must be in the db. "
               << "odb error is: " << e.what();
  }

  auto it = db_map_.find(db_schema_name);
  if (it == db_map_.end()) {
    LOG(FATAL) << "The database type '" << db_schema_name << "' is not found."
               << std::endl
               << "Try to insert this typename via plugin.";
  }
  return it->second->Read(filename);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Database * syd::DatabaseManager::Create(const std::string & db_schema_name,
                                             const std::string & filename,
                                             const std::string & folder)
{
  auto it = db_map_.find(db_schema_name);
  auto & list = GetDatabaseSchemas();
  if (it == db_map_.end()) {
    std::stringstream os;
    for(auto i=list.begin(); i != list.end(); i++) {
      os << *i << " ";
    }
    LOG(FATAL) << "The database type '" << db_schema_name << "' is not found."
               << std::endl
               << "Known type are: " << os.str() << std::endl
               << "Try to insert this typename via plugin.";
  }
  return it->second->Create(db_schema_name, filename, folder);
}
// --------------------------------------------------------------------
