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
#include "sydDatabase.h"

// --------------------------------------------------------------------
// http://stackoverflow.com/questions/1607368/sql-query-logging-for-sqlite
void trace_callback( void* udp, const char* sql ) {
  syd::Database * d = static_cast<syd::Database*>(udp);
  //  printf("{SQL} [%s]\n", sql);
  d->TraceCallback(sql);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Database::Database(std::string name):
  name_(name)
{
  filename_ = "filename_not_set";
  folder_ = "folder_not_set";
  current_sql_query_ = "no_query";
  db_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::TraceCallback(const char* sql)
{
  std::string s = std::string(sql);
  if (s == "COMMIT") return;
  if (s == "BEGIN") return;
  //  std::cout << s << std::endl;
  LOG(INFO) << s;
  current_sql_query_=s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::SetFileAndFolder(std::istringstream & f)
{
  if (!getline(f, filename_, ';')) {
    LOG(FATAL) << "Error while parsing db params for filename. db is "
               << get_name() << " (" << get_typename() << ") params = " << f.str();
  }
  if (!getline(f, folder_, ';')) {
    LOG(FATAL) << "Error while parsing db params for folder. db is "
               << get_name() << " (" << get_typename() << ") params = " << f.str();
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// static function
std::shared_ptr<syd::Database> syd::Database::OpenDatabase(std::string name, std::string init_filename)
{
  syd::DatabaseFactory::OpenDatabaseFilenames(init_filename);
  std::string type_name;
  std::string param;
  syd::DatabaseFactory::GetTypeAndParamFromName(name, type_name, param);
  return syd::DatabaseFactory::Instance()->NewDatabase(type_name, name, param);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::OpenSqliteDatabase(std::string filename, std::string folder)
{
  // Open the DB
  try {
    db_ = new odb::sqlite::database(filename);
    odb::connection_ptr c(db_->connection());
    c->execute("PRAGMA foreign_keys=ON;");
  }
  catch (const odb::exception& e) {
    LOG(FATAL) << "Cannot open db '" << filename << "' : " << e.what();
  }
  filename_ = filename;

  // Check folder
  folder_ = folder+PATH_SEPARATOR;
  if (!syd::DirExists(folder)) {
    LOG(FATAL) << "The folder '" << folder << "' does not exist.";
  }

  // Verbose
  VLOG(3) << "Opening db " << get_name() << " (" << get_typename() << ") : "
          << get_filename() << " " << folder_;

  // Install tracer
  odb::sqlite::connection_ptr c (db_->connection ());
  sqlite3* handle (c->handle ());
  sqlite3_trace (handle, trace_callback, this);
}
// --------------------------------------------------------------------
