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

// --------------------------------------------------------------------
template<class DatabaseSchema>
DatabaseCreator<DatabaseSchema> * syd::DatabaseManager::RegisterDatabaseSchema(const std::string & schema)
{
  auto it = db_map_.find(schema);
  if (it != db_map_.end()) {
    LOG(FATAL) << "The database schema '" << schema << "' already exist.";
  }
  DatabaseCreator<DatabaseSchema> * c = new DatabaseCreator<DatabaseSchema>;
  db_map_[schema] = c;
  db_schema_names_.push_back(schema);
  LOG(5) << "The database schema '" << schema << "' is registered.";
  c->AddSchemaName(schema);
  return c;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class DatabaseSchema>
DatabaseSchema * syd::DatabaseManager::Open(const std::string & filename)
{
  syd::Database * db = Open(filename);
  // Check it the db inherit (or is) a DatabaseSchema;
  if (dynamic_cast<DatabaseSchema*>(db) == 0) {
    EXCEPTION("Error the db '" << filename
               << "' does not contains the required schema. The schema in the file is '"
               << db->GetDatabaseSchema()  << "'.");
  }
  return static_cast<DatabaseSchema*>(db);
}
// --------------------------------------------------------------------
