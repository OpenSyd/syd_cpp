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
#include "sydTable.h"

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
  try {
    LOG(4) << "Opening database '" << filename_ << "'.";
    db_ = new odb::sqlite::database(filename_);
    odb::connection_ptr c(db_->connection());
    c->execute("PRAGMA foreign_keys=ON;");
  }
  catch (const odb::exception& e) {
    LOG(FATAL) << "Cannot open db '" << filename_ << "' : " << e.what();
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
    LOG(FATAL) << "Could not get the folder name ?" << e.what();
  }

  // Now consider the folder according to the filename path and check it exists
  std::string pwd;
  if (!syd::GetWorkingDirectory(pwd))  {
    LOG(FATAL) << "Error while trying to get current working dir.";
  }
  if (filename_[0] != PATH_SEPARATOR) {
    pwd = pwd+PATH_SEPARATOR+filename_;
    unsigned found = pwd.find_last_of(PATH_SEPARATOR);
    pwd = pwd.substr(0,found);
  }
  absolute_folder_ = pwd+PATH_SEPARATOR+relative_folder_;
  if (!syd::DirExists(absolute_folder_)) {
    LOG(FATAL) << "The folder '" << absolute_folder_ << "' does not exist.";
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
syd::TableElement * syd::Database::InsertFromArg(const std::string & table_name, std::vector<std::string> & arg) {
  return GetTable(table_name)->InsertFromArg(arg);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Dump(const std::vector<std::string> & args, std::ostream & os)
{
  if (args.size() == 0) {
    os << "Database schema: " << GetDatabaseSchema() << std::endl;
    os << "Database folder: " << GetRelativeDBFolder() << std::endl;
    for(auto i=map.begin(); i != map.end(); i++) {
      int n = i->second->GetNumberOfElements();
      os << "Table \t" << std::setw(15) << i->first << " " <<  std::setw(10) << n;
      if (n>1) os << " elements" << std::endl;
      else os << " element" << std::endl;
    }
    os << std::flush;
    return;
  }
  if (args[0] == "table") {
    if (args.size() < 2) {
      LOG(FATAL) << "Please provide the name of the table you want to dump.";
    }
    DumpTable(args[1], os);
    return;
  }
  DumpTable(args[0], os); // suppose it is a table name
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::DumpTable(const std::string & table_name, std::ostream & os)
{
  GetTable(table_name)->DumpTable(os);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TableBase * syd::Database::GetTable(const std::string & table_name)
{
  std::string str=table_name;
  std::transform(str.begin(), str.end(),str.begin(), ::tolower);
  auto it = map_lowercase.find(str);
  if (it == map_lowercase.end()) {
    LOG(FATAL) << "Cannot find the table '" << table_name << "'.";
    return 0; // to avoid warning
  }
  return it->second;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Database::GetListOfTableNames()
{
  std::stringstream os;
  for(auto i = map.begin(); i!=map.end(); i++) {
    os << i->first << " ";
  }
  return os.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::ExecuteSQL(const std::string & statement)
{
  try {
    odb::transaction t (db_->begin());
    db_->execute(statement);
    t.commit();
  }
  catch (const odb::exception& e) {
    LOG(FATAL) << "Could not execute native SQL: " << statement
               << std::endl << e.what();
  }
}
// --------------------------------------------------------------------
