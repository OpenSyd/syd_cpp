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
#include "sydException.h"

//syd::DatabaseManager * syd::DatabaseManager::singleton_ = new syd::DatabaseManager;
//extern syd::DatabaseManager * syd::DatabaseManager::singleton_;

// --------------------------------------------------------------------
syd::DatabaseManager * syd::DatabaseManager::GetInstance()
{
  // http://stackoverflow.com/questions/2505385/classes-and-static-variables-in-shared-libraries
  static syd::DatabaseManager * singleton_ = NULL;
  if (singleton_ == NULL) {
    singleton_ = new DatabaseManager;
    std::set_terminate(syd::terminateHandler);
  }
  return singleton_;
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
syd::DatabaseManager::~DatabaseManager()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::map<odb::database *, syd::Database *> &
syd::DatabaseManager::GetListOfLoadedDatabases()
{
  // http://stackoverflow.com/questions/2505385/classes-and-static-variables-in-shared-libraries
  static std::map<odb::database *, syd::Database *> list;
  return list;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Database * syd::DatabaseManager::Open(std::string filename)
{
  // Get the real filename from the environment variable is filename is 'default'
  if (filename == "default") {
    char * file = getenv("SYD_CURRENT_DB");
    if (file == NULL) {
      LOG(FATAL) << "Cannot find the SYD_CURRENT_DB environment variable. Please set with something like: "
                 << "export SYD_CURRENT_DB=/home/genghiskhan/my_db/toto.db";
    }
    filename = std::string(file);
  }

  // Create and open the db
  odb::sqlite::database * db;
  LOG(5) << "Opening database '" << filename << "' to get the type of db.";
  try {
    db = new odb::sqlite::database(filename);
    odb::connection_ptr c(db->connection());
  }
  catch (const std::exception& e) {
    EXCEPTION("Cannot open db '" << filename << "' : " << e.what());
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
      EXCEPTION("Cannot find schema_version name in the db " << filename);
    }

    // Only *first* database_schema
    r.begin().load(s);
    /*
    // We only consider the *last* element here. The table
    // schema_version can contains several schema names, but only the
    // last defined the database type.
    for(auto i=r.begin(); i != r.end(); i++) {
    r.begin().load(s);
    DD(s.database_schema);
    }
    */
    transaction.commit();
    db_schema_name = s.database_schema;
    LOG(5) << "Database type is '" << db_schema_name << "'.";
  }
  catch (const odb::exception& e) {
    EXCEPTION("Cannot find the db typename in '" << filename << "'."
              << std::endl << "A table name 'schema_version' must be in the db. "
              << "odb error is: " << e.what());
  }

  auto it = db_map_.find(db_schema_name);
  if (it == db_map_.end()) {
    EXCEPTION("The database type '" << db_schema_name << "' read in "
              << filename << " is not found. " << std::endl
              << "Try to insert this typename via plugin.");
  }
  return it->second->Open(filename);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DatabaseManager::Create(const std::string & db_schema_name,
                                  const std::string & filename,
                                  const std::string & folder,
                                  bool force_overwrite)
{
  auto it = db_map_.find(db_schema_name);
  auto & list = GetDatabaseSchemas();
  if (it == db_map_.end()) {
    std::stringstream os;
    os << " ";
    for(auto i=list.begin(); i != list.end(); i++) {
      os << *i << " ";
    }
    EXCEPTION("The database type '" << db_schema_name << "' is not found."
              << std::endl
              << "Known type are: " << os.str() << std::endl
              << "Try to insert this typename via plugin.");
  }
  it->second->Create(db_schema_name, filename, folder, force_overwrite);
}
// --------------------------------------------------------------------
