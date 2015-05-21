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
template<class DatabaseType>
Database * syd::DatabaseCreator<DatabaseType>::Read(std::string filename)
{
  syd::Database * db = new DatabaseType();
  db->Read(filename);
  return db;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class DatabaseType>
Database* syd::DatabaseCreator<DatabaseType>::Create(std::string dbtype,
                                                     std::string filename,
                                                     std::string folder)
{
  // Create schema
  odb::sqlite::database db(filename, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, false);
  try {
    odb::connection_ptr c (db.connection ());
    c->execute ("PRAGMA foreign_keys=ON");
    odb::transaction t (db.begin ());
    odb::schema_catalog::create_schema(db, "sydCommonSchema"); // common schema
    // Specific list of schemas
    for(auto s:schemas) odb::schema_catalog::create_schema(db, s);
    t.commit ();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Cannot create the database '" << filename << "' of type '"
              << dbtype << "', error is: " << e.what());
  }

  // Insert the information
  syd::DatabaseInformation s;
  s.folder = folder;
  s.database_schema = dbtype;
  try {
    odb::transaction t (db.begin());
    db.persist(s);
    db.update(s);
    t.commit();
  } catch (const odb::exception& e) {
    EXCEPTION("Error while trying to insert basic database information (sydCommonSchema) in the db. Error is:" << e.what());
  }

  return Read(filename);
}
// --------------------------------------------------------------------
