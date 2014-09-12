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
syd::Database::Database()
{
  filename_ = "filename_not_set";
  folder_ = "folder_not_set";
  current_sql_query_ = "no_query";
  db = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Database::~Database()
{
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
void syd::Database::OpenDatabase()
{
  // Get DB filename
  char * bdb = getenv ("SYD_DB");
  if (bdb == NULL) LOG(FATAL) << "please set SYD_DB environment variable.";
  std::string filename = std::string(bdb);

  // Get Database folder
  char * b =getenv ("SYD_IMAGE_FOLDER");
  if (b == NULL) LOG(FATAL) << "please set SYD_IMAGE_FOLDER environment variable.";
  std::string folder = std::string(b)+"/";

  // Open
  OpenDatabase(filename, folder);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::OpenDatabase(std::string filename, std::string folder)
{
  // Open the DB
  try {
    db = new odb::sqlite::database(filename);
    odb::connection_ptr c(db->connection());
    c->execute("PRAGMA foreign_keys=ON;");
  }
  catch (const odb::exception& e) {
    LOG(FATAL) << "Cannot open db '" << filename << "' : " << e.what();
  }
  filename_ = filename;

  // Check folder
  folder_ = folder+PATH_SEPARATOR;
  if (!OFStandard::dirExists(folder.c_str())) {
    LOG(FATAL) << "The folder '" << folder << "' does not exist.";
  }

  // Verbose
  VLOG(2) << "Openining the database " << filename_;
  VLOG(2) << "With the folder " << folder_;

  // Install tracer
  odb::sqlite::connection_ptr c (db->connection ());
  sqlite3* handle (c->handle ());
  sqlite3_trace (handle, trace_callback, this);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------

// --------------------------------------------------------------------
