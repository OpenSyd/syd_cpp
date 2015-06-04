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
  SetDeleteDryRunFlag(false);
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
void syd::Database::Dump(const std::vector<std::string> & args, std::ostream & os)
{
  if (args.size() == 0) {
    os << "Database schema: " << GetDatabaseSchema() << std::endl;
    os << "Database folder: " << GetDBFolder() << std::endl;
    for(auto i=map.begin(); i != map.end(); i++) {
      int n = i->second->GetNumberOfElements();
      os << "Table \t" << std::setw(15) << i->first << " " <<  std::setw(10) << n;
      if (n>1) os << " elements" << std::endl;
      else os << " element" << std::endl;
    }
    os << std::flush;
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
void syd::Database::CopyDatabaseTo(std::string file, std::string folder)
{
  // Copy db
  syd::CopyFile(GetFilename(), file);

  // Create folder
  if (!syd::DirExists(folder)) syd::CreateDirectory(folder);

  // open the copied db and change the folder name
  try {
    odb::sqlite::database db(file, SQLITE_OPEN_READWRITE, false);
    odb::transaction transaction (db.begin());
    typedef odb::query<syd::DatabaseInformation> query;
    typedef odb::result<syd::DatabaseInformation> result;
    query q;
    result r (db.query<syd::DatabaseInformation>(q));
    syd::DatabaseInformation s;
    r.begin().load(s);
    s.folder = folder;
    db.update(s);
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Could not change the folder name in " << file << ". ODB error is: " << e.what());
  }

  // Copy the folder content :/ FIXME (not on windows !)
  std::ostringstream cmd;
  cmd << "cp -r " << GetAbsoluteDBFolder() << "/* " << folder;
  system(cmd.str().c_str());

  // if (!syd::DirExists(folder)) syd::CreateDirectory(folder);
  // OFString scanPattern = "*";
  // OFString dirPrefix = "";
  // OFBool recurse = OFTrue;
  // size_t found=0;
  // OFList<OFString> & inputFiles;
  // found = OFStandard::searchDirectoryRecursively(GetAbsoluteFolder().c_str(),
  //                                                inputFiles, scanPattern,
  //                                                dirPrefix, recurse);
  // for(auto f:inputFiles) syd::CopyFile(

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
void syd::Database::DeleteCurrentList()
{
  odb::connection_ptr c (db_->connection ());
  c->execute ("PRAGMA foreign_keys=ON");
  odb::transaction t (db_->begin());

  // Delete all the elements in the list
  /*
  for(auto it=list_of_elements_to_delete_.begin();
      it != list_of_elements_to_delete_.end(); ++it) {
    std::string table_name = it->first;
    if (!delete_dry_run_flag_) GetTable(table_name)->Erase(it->second);
    LOG(2) << "Deleting " << it->first << " " << *it->second;
  }
  */
  for(auto it=list_of_elements_to_delete_.begin();
      it != list_of_elements_to_delete_.end(); ++it) {
    auto & p = it->second;
    if (!delete_dry_run_flag_) GetTable(p.first)->Erase(p.second);
  }


  t.commit();

  // Verbose
  if (delete_dry_run_flag_) {
    LOG(WARNING) << "*Dry run*: " << list_of_elements_to_delete_.size() << " records would have been deleted.";
  }
  else {
    LOG(1) << list_of_elements_to_delete_.size() << " records have been deleted.";
  }

  // free memory
  for(auto it=list_of_elements_to_delete_.begin();
      it != list_of_elements_to_delete_.end(); ++it) {
    //    delete it->second;
    delete it->second.second;
  }
  list_of_elements_to_delete_.clear();
}
// --------------------------------------------------------------------
