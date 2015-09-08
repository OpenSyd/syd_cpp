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
void syd::DatabaseCreator<DatabaseType>::Create(std::string dbtype,
                                                std::string filename,
                                                std::string folder,
                                                bool force_overwrite)
{
  // Check folder
  if (folder.find(PATH_SEPARATOR) != std::string::npos) {
    LOG(FATAL) << "The folder must be a simple folder name, without path or subfolder.";
  }

  if (fs::exists(filename)) {
    if (!force_overwrite) {
      LOG(FATAL) << "Cannot create the database, the file '" << filename << "' already exists.";
    }
    fs::rename(filename, filename+".backup");
  }

  // Create folder
  std::string f = syd::GetPathFromFilename(filename)+PATH_SEPARATOR+folder;
  if (!fs::exists(f)) {
    LOG(WARNING) << "The folder '" << f << "' does not exist, I create it.";
    fs::create_directories(f);
  }

  // Create schema
  odb::sqlite::database db(filename, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, false);
  try {
    odb::connection_ptr c (db.connection ());
    c->execute ("PRAGMA foreign_keys=ON");
    odb::transaction t (db.begin ());
    odb::schema_catalog::create_schema(db, "sydCommonDatabase"); // common schema
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
    EXCEPTION("Error while trying to insert basic database information (sydCommonDatabase) in the db. Error is:" << e.what());
  }
}
// --------------------------------------------------------------------
