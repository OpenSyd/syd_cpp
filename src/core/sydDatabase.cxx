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
  d->TraceCallback(sql);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Database::Database()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Read(std::string filename)
{
  filename_ = filename;
  // Open the DB
  LOG(5) << "Opening database '" << filename_ << "'.";
  try {
    db_ = new odb::sqlite::database(filename_);
    odb::connection_ptr c(db_->connection());
    c->execute("PRAGMA foreign_keys=ON;");
  }
  catch (const odb::exception& e) {
    EXCEPTION("Cannot open db '" << filename_ << "' : " << e.what());
  }

  // Set the folder by reading the 'folder' value in the db_info table.
  try {
    odb::transaction transaction (db_->begin());
    typedef odb::query<syd::DatabaseInformation> query;
    typedef odb::result<syd::DatabaseInformation> result;
    query q;
    result r (db_->query<syd::DatabaseInformation>(q));
    syd::DatabaseInformation s;
    r.begin().load(s);
    relative_folder_ = s.folder;
    database_schema_ = s.database_schema;
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Could not read the folder name in the database ?" << e.what());
  }

  // Now consider the folder according to the filename path and check it exists
  std::string pwd = filename;
  ConvertToAbsolutePath(pwd);
  pwd = GetPathFromFilename(pwd);

  // if (!syd::GetWorkingDirectory(pwd))  {
  //   EXCEPTION("Error while trying to get current working dir.");
  // }
  // if (filename_[0] != PATH_SEPARATOR) {
  //   pwd = pwd+PATH_SEPARATOR+filename_;
  //   unsigned found = pwd.find_last_of(PATH_SEPARATOR);
  //   pwd = pwd.substr(0,found);
  // }

  absolute_folder_ = pwd+PATH_SEPARATOR+relative_folder_;
  if (!syd::DirExists(absolute_folder_)) {
    EXCEPTION("The folder '" << absolute_folder_ << "' does not exist.");
  }

  // Install tracer
  odb::sqlite::connection_ptr c (db_->connection ());
  sqlite3* handle (c->handle ());
  sqlite3_trace (handle, trace_callback, this);

  // Define the tables
  CreateTables();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::TraceCallback(const char* sql)
{
  std::string s = std::string(sql);
  if (s == "COMMIT") return;
  if (s == "BEGIN") return;
  LOGSQL << s;
  current_sql_query_=s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*void syd::Database::AddTableT(const std::string & tablename, syd::TableBase * table)
{
  // No exception handling here, fatal error if fail.
  if (db_ == NULL) {
    LOG(FATAL) << "Could not AddTable, open a db before";
  }

  std::string str = tablename;
  std::transform(str.begin(), str.end(),str.begin(), ::tolower);
  auto it = map_lowercase.find(str);
  if (it != map_lowercase.end()) {
    LOG(FATAL) << "When creating the database, a table with the same name '" << tablename
               << "' already exist.";
  }
  table->SetSQLDatabase(db_);
  //  table->SetDatabase(this);
  map[tablename] = table;
  map_lowercase[str] = map[tablename];
}
*/
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Dump(std::ostream & os) const
{
  os << "Database schema: " << GetDatabaseSchema() << std::endl;
  os << "Database folder: " << GetDatabaseRelativeFolder() << std::endl;
  // for(auto i=map.begin(); i != map.end(); i++) {
  //   int n = i->second->GetNumberOfElements();
  //   os << "Table \t" << std::setw(15) << i->first << " " <<  std::setw(10) << n;
  //   if (n>1) os << " elements" << std::endl;
  //   else os << " element" << std::endl;
  // }
  os << std::flush;
}
// --------------------------------------------------------------------


// void syd::Database::Insert2(std::shared_ptr<syd::Record> record)
// {
//   DD("Insert2");
//   odb::transaction t (db_->begin());
//   db_->persist(record);
//   //    db_->update(r);
//   t.commit();
// }

// --------------------------------------------------------------------
/*
syd::TableBase * syd::Database::GetTable(const std::string & table_name) const
{
  std::string str=table_name;
  std::transform(str.begin(), str.end(),str.begin(), ::tolower);
  auto it = map_lowercase.find(str);
  if (it == map_lowercase.end()) {
    EXCEPTION("Cannot find the table '" << table_name << "'." << std::endl
              << "Existing tables are: " << GetListOfTableNames());
  }
  return it->second;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Database::GetListOfTableNames() const
{
  std::stringstream os;
  for(auto i = map.begin(); i!=map.end(); i++) {
    os << i->first << " ";
  }
  return os.str();
}
*/
// --------------------------------------------------------------------
