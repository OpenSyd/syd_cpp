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
  SetDeleteForceFlag(true); // by default, dont ask before deleting
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
  std::string pwd;
  if (!syd::GetWorkingDirectory(pwd))  {
    EXCEPTION("Error while trying to get current working dir.");
  }
  if (filename_[0] != PATH_SEPARATOR) {
    pwd = pwd+PATH_SEPARATOR+filename_;
    unsigned found = pwd.find_last_of(PATH_SEPARATOR);
    pwd = pwd.substr(0,found);
  }
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
syd::TableElementBase * syd::Database::InsertFromArg(const std::string & table_name,
                                                     std::vector<std::string> & arg)
{
  return GetTable(table_name)->InsertFromArg(arg);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Dump(std::ostream & os)
{
  os << "Database schema: " << GetDatabaseSchema() << std::endl;
  os << "Database folder: " << GetDatabaseRelativeFolder() << std::endl;
  for(auto i=map.begin(); i != map.end(); i++) {
    int n = i->second->GetNumberOfElements();
    os << "Table \t" << std::setw(15) << i->first << " " <<  std::setw(10) << n;
    if (n>1) os << " elements" << std::endl;
    else os << " element" << std::endl;
  }
  os << std::flush;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Dump(std::ostream & os, const std::string & table_name, const std::string & format)
{
  GetTable(table_name)->Dump(os, format);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Dump(std::ostream & os,
                         const std::string & table_name,
                         const std::string & format,
                         const std::vector<IdType> & ids)
{
  GetTable(table_name)->Dump(os, format, ids);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Find(std::vector<syd::IdType> & ids,
                         const std::string & table_name,
                         const std::vector<std::string> & pattern,
                         const std::vector<std::string> & exclude)

{
  GetTable(table_name)->Find(ids, pattern, exclude);
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
syd::TableBase * syd::Database::GetTable(const std::string & table_name)
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
void syd::Database::Delete(const std::string & table_name, syd::IdType id)
{
  GetTable(table_name)->AddToDeleteList(id);
  DeleteCurrentList();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Delete(const std::string & table_name, std::vector<syd::IdType> & ids)
{
  GetTable(table_name)->AddToDeleteList(ids);
  DeleteCurrentList();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::DeleteAll(const std::string & table_name)
{
  GetTable(table_name)->AddAllToDeleteList();
  DeleteCurrentList();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::OnDelete(const std::string & table_name, TableElementBase * elem)
{
  // to be overloaded
  elem->OnDelete(this);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::Database::DeleteCurrentList()
{
  if (list_of_elements_to_delete_.size() == 0) {
    LOG(1) << "No record have been deleted.";
    return false;
  }

  if (!deleteForceFlag_) {
    std::string input;
    std::cout << "Really delete " << list_of_elements_to_delete_.size() << " elements ([y]es/[n]o/[v]erbose) ?   ";
    std::cout.flush();
    std::getline(std::cin, input);
    if (input == "v") {
      for(auto it=list_of_elements_to_delete_.begin();
          it != list_of_elements_to_delete_.end(); ++it) {
        auto & p = it->second;
        std::cout << "\t delete: " << p.first << ": " << *p.second << std::endl;
      }
      return DeleteCurrentList();
    }
    if (input != "y") {
      LOG(1) << "No record have been deleted.";
      return false;
    }
  }

  // Start the deletion
  odb::connection_ptr c (db_->connection ());
  c->execute ("PRAGMA foreign_keys=ON");
  odb::transaction t (db_->begin());

  for(auto it=list_of_elements_to_delete_.begin();
      it != list_of_elements_to_delete_.end(); ++it) {
    auto & p = it->second;
    GetTable(p.first)->Erase(p.second);
  }
  t.commit();

  // Verbose
  LOG(1) << list_of_elements_to_delete_.size() << " records have been deleted.";

  // free memory
  for(auto it=list_of_elements_to_delete_.begin();
      it != list_of_elements_to_delete_.end(); ++it) {
    delete it->second.second;
  }
  list_of_elements_to_delete_.clear();
  return true;
}
// --------------------------------------------------------------------
