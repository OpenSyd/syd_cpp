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
#include "sydFileUtils.h"
#include "sydPluginManager.h"

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
  description_ = NULL;
  overwrite_file_if_exists_flag_ = true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Database::~Database()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::OpenFromFile(std::string filename)
{
  filename_ = filename;
  // Open the DB
  LOG(5) << "Opening database '" << filename_ << "'.";
  try {
    //    odb_db_ = std::unique_ptr<odb::sqlite::database>(new odb::sqlite::database(filename_));
    odb_db_ = new odb::sqlite::database(filename_, SQLITE_OPEN_READWRITE, true); // true = foreign_keys
    // odb::connection_ptr c(odb_db_->connection());
    // c->execute("PRAGMA foreign_keys=ON;");
  }
  catch (const odb::exception& e) {
    EXCEPTION("Cannot open db '" << filename_ << "' : " << e.what());
  }

  // Set the folder by reading the 'folder' value in the odb_db_info table.
  try {
    odb::transaction transaction (odb_db_->begin());
    typedef odb::query<syd::DatabaseInformation> query;
    typedef odb::result<syd::DatabaseInformation> result;
    query q;
    result r (odb_db_->query<syd::DatabaseInformation>(q));
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
  std::string database_path = filename;
  syd::ConvertToAbsolutePath(database_path);
  database_path = GetPathFromFilename(database_path);

  absolute_folder_ = database_path+PATH_SEPARATOR+relative_folder_;
  if (!fs::exists(absolute_folder_)) {
    //EXCEPTION("The folder '" << absolute_folder_ << "' does not exist.");
    LOG(WARNING) << "The folder '" << absolute_folder_ << "' does not exist.";
  }

  // Install tracer
  odb::sqlite::connection_ptr c (odb_db_->connection ());
  sqlite3* handle (c->handle ());
  sqlite3_trace (handle, trace_callback, this);

  // Check version etc
  odb::schema_version file_version (odb_db_->schema_version(GetDatabaseSchema()));
  odb::schema_version current_version
    (odb::schema_catalog::current_version (*odb_db_, GetDatabaseSchema()));
  odb::schema_version base_version
    (odb::schema_catalog::base_version (*odb_db_, GetDatabaseSchema()));

  if (file_version < current_version) { // should migrate ?
    LOG(WARNING) << "The version of the db schema in the file " << filename
                 << " is " << GetVersionAsString(file_version)
                 << " while the current version is "
                 << GetVersionAsString(current_version) << std::endl
                 << "You need db migration (but it is not always possible). "
                 << "Should I try (a backup is made before) ? ";
    char c;
    std::scanf("%c", &c);
    if (c =='y') MigrateSchema();
    else {
      LOG(FATAL) << "Abort.";
    }
  }
  if (file_version > current_version) {
    LOG(FATAL) << "The database version in the file is newer ("
               << GetVersionAsString(file_version)
               << ") than the syd database version ("
               << GetVersionAsString(current_version)
               << "), upgrade syd.";
  }

  // Define the tables
  CreateTables();

  // Register to the loaded database
  syd::DatabaseManager::GetListOfLoadedDatabases()[odb_db_] = this;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::TraceCallback(const char* sql)
{
  std::string s = std::string(sql);
  if (s == "COMMIT") return;
  if (s == "BEGIN") return;
  LOG_SQL << s;
  current_sql_query_=s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Dump(std::ostream & os)
{
  os << "Database file  : " << GetFilename() << std::endl;
  os << "Database schema: " << GetDatabaseSchema() << std::endl;
  os << "Database folder: " << GetDatabaseRelativeFolder();
  if (!fs::exists(GetDatabaseAbsoluteFolder()))
    os << warningColor << " -> does not exist ("
       << GetDatabaseAbsoluteFolder() << ")" << resetColor;
  os << std::endl;
  for(auto i=map_.begin(); i != map_.end(); i++) {
    int n = GetNumberOfElements(i->first);
    os << "Table: " << std::setw(15) << i->first << " " <<  std::setw(10) << n;
    if (n>1) os << " elements" << std::endl;
    else os << " element" << std::endl;
  }
  os << std::flush;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Database::ConvertToAbsolutePath(std::string relative_path) const
{
  // remove duplicate '/'
  std::string s = absolute_folder_+PATH_SEPARATOR+relative_path;
  std::string p;
  p += PATH_SEPARATOR;
  p += PATH_SEPARATOR;
  std::size_t found = std::string::npos;
  do {
    found = s.find(p);
    if (found != std::string::npos) s.erase(found, 1);

  } while (found != std::string::npos);
  return s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Database::GetUniqueTempFilename() const
{
  std::string t = GetDatabaseAbsoluteFolder()+PATH_SEPARATOR
    +"syd_temp_%%%%_%%%%_%%%%_%%%%.mhd";
  fs::path p = fs::unique_path(t);
  return p.string();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::CheckOrCreateRelativePath(std::string relative_path)
{
  // Check directory or create it
  std::string absolute_folder = ConvertToAbsolutePath(relative_path);
  fs::path dir(absolute_folder);
  if (!fs::exists(dir)) {
    LOG(4) << "Creating folder: " << absolute_folder;
    fs::create_directories(dir);
    if (!fs::exists(dir)) {
      EXCEPTION("Error, could not create the folder: " << absolute_folder);
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Dump(const std::string & table_name,
                         const std::string & format,
                         std::ostream & os)
{
  syd::Record::vector records;
  Query(records, table_name); // get all records
  Dump(records, format, os);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Insert(generic_record_pointer record)
{
  GetTable(record->GetTableName())->Insert(record);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Insert(generic_record_vector records,
                           const std::string & table_name)
{
  if (records.size() == 0) return;
  GetTable(table_name)->Insert(records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Update(generic_record_pointer record)
{
  GetTable(record->GetTableName())->Update(record);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Update(generic_record_vector records,
                           const std::string & table_name)
{
  if (records.size() == 0) return;
  GetTable(table_name)->Update(records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TableBase * syd::Database::GetTable(const std::string & table_name) const
{
  std::string str=table_name;
  std::transform(str.begin(), str.end(),str.begin(), ::tolower);
  auto it = map_lowercase_.find(str);
  if (it == map_lowercase_.end()) {
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
  for(auto i = map_.begin(); i!=map_.end(); i++) {
    os << i->first << " ";
  }
  return os.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Database::generic_record_pointer
syd::Database::New(const std::string & table_name)
{
  return GetTable(table_name)->New();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void  syd::Database::QueryOne(generic_record_pointer & record,
                              const std::string & table_name,
                              const IdType & id) const
{
  GetTable(table_name)->QueryOne(record, id);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Query(generic_record_vector & records,
                          const std::string table_name,
                          const std::vector<syd::IdType> & ids) const
{
  GetTable(table_name)->Query(records, ids);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Query(generic_record_vector & records,
                          const std::string table_name) const
{
  GetTable(table_name)->Query(records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
long syd::Database::GetNumberOfElements(const std::string & table_name)
{
  // native query
  auto tdesc = GetTableDescription(table_name);
  auto table_sql_name = AddDoubleQuoteAround(tdesc->GetSQLTableName());
  std::ostringstream sql;
  sql << "SELECT COUNT(*) FROM " << table_sql_name;

  sqlite3 * sdb = GetSqliteHandle();
  sqlite3_stmt * stmt;
  auto rc = sqlite3_prepare_v2(sdb, sql.str().c_str(), -1, &stmt, NULL);
  long nb = 0;
  if (rc==SQLITE_OK) {
    /* Loop on result with the following structure:
       TABLE sqlite_master
       type TEXT, name TEXT, tbl_name TEXT, rootpage INTEGER, sql TEXT  */
    while(sqlite3_step(stmt) == SQLITE_ROW) {
      nb = sqlite3_column_int(stmt, 0);
    }
  }
  else {
    EXCEPTION("Could not retrieve the list of tables in the db");
  }
  return nb;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Sort(generic_record_vector & records,
                         const std::string & table_name,
                         const std::string & type) const
{
  if (records.size() == 0) return;
  GetTable(table_name)->Sort(records, type);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Delete(generic_record_vector & records,
                           const std::string & table_name)
{
  if (records.size() == 0) return;
  GetTable(table_name)->Delete(records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Delete(generic_record_pointer record)
{
  generic_record_vector r;
  r.push_back(record);
  Delete(r, record->GetTableName());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::DeleteFiles()
{
  for(auto f:files_to_delete_) {
    if (std::remove(f.c_str()) != 0) {
      LOG(WARNING) << "Could not delete the file " << f;
    }
  }
  files_to_delete_.clear();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::AddFilenameToDelete(const std::string & f)
{
  files_to_delete_.push_back(f);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Update(generic_record_pointer & record,
                           std::string field_name,
                           std::string value)
{
  std::string table_name = record->GetTableName();
  auto desc = GetDatabaseDescription();
  syd::TableDescription * tdesc;
  bool b = desc->FindTableDescription(table_name, &tdesc);
  if (!b) EXCEPTION("Could not find the table " << table_name);
  syd::FieldDescription * field;
  b = tdesc->FindField(field_name, &field);
  if (!b) EXCEPTION("Could not find the field " << field_name);

  std::string table_sql_name = AddDoubleQuoteAround(field->GetSQLTableName());
  std::string field_sql_name = AddDoubleQuoteAround(field->GetName());
  std::string v = value;

  std::ostringstream sql;
  sql << "UPDATE " << table_sql_name
      << " SET " << field_sql_name << " = \"" << v << "\""
      << " WHERE id=" << record->id;

  try {
    odb::transaction t (odb_db_->begin ());
    odb_db_->execute (sql.str());
    t.commit ();
  } catch (const odb::exception& e) {
    EXCEPTION("Error during the following sql query: " << std::endl
              << sql.str() << std::endl
              << "Error is:" << e.what());
  }
  QueryOne(record, table_name, record->id);
}
// --------------------------------------------------------------------


//---------------------------------------------------------------------
std::string syd::sqlite3_column_text_string(sqlite3_stmt * stmt, int iCol)
{
  return reinterpret_cast<const char*>(sqlite3_column_text(stmt, iCol));
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
sqlite3 * syd::Database::GetSqliteHandle() const
{
  odb::sqlite::connection_ptr c (odb_db_->connection());
  sqlite3 * sdb(c->handle());
  return sdb;
}
//---------------------------------------------------------------------


// --------------------------------------------------------------------
//FIXME TO REMOVE ?
void syd::Database::CheckDatabaseSchema()
{
  // FIXME -> check it is already open

  // Read sql db schema
  auto sql_desc = new syd::DatabaseDescription; // sql db description
  sql_desc->ReadDatabaseSchema(this);

  // Check
  auto desc = GetDatabaseDescription(); // OO db description
  for(auto t:desc->GetTablesDescription()) {
    if (t->GetTableName() == "Record") continue; // FIXME temporary, to remove !
    for(auto f:t->GetFields()) {
      syd::TableDescription * d;
      bool b = sql_desc->FindTableDescription(f->GetSQLTableName(), &d);
      if (!b) {
        LOG(FATAL) << "The table '"
                   << f->GetSQLTableName()
                   << "' is needed and not found in the db. You should migrate the db. ";
      }
      else {
        syd::FieldDescription * field;
        b = d->FindField(f->GetName(), &field);
        if (!b) {
          LOG(FATAL) << "The field '"
                     << f->GetName()
                     << "' is needed and not found in the db. You should migrate the db. ";
        }
        else {
          LOG(1) << t->GetTableName() << "." << f->GetName() << " is found (in "
                 << f->GetSQLTableName() << "." << f->GetName() << ")";
        }
      }
    }
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DatabaseDescription * syd::Database::GetDatabaseDescription()
{
  if (description_ == NULL) {
    description_ = new DatabaseDescription();
    description_->Init(this);
  }
  return description_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TableDescription * syd::Database::GetTableDescription(const std::string & table_name)
{
  auto desc = GetDatabaseDescription();
  syd::TableDescription * tdesc;
  bool b = desc->FindTableDescription(table_name, &tdesc);
  if (!b) EXCEPTION("Could not find the table " << table_name);
  return tdesc;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::MigrateSchema()
{
  // First make a copy !
  LOG(0) << "Backup current file";
  std::string backup = GetFilename()+".backup";
  while (fs::exists(backup)) {
    backup = backup+".backup";
  }
  fs::copy_file(GetFilename(), backup);

  // Retrieve the schemas hierarchy
  std::vector<std::string> schema_names;
  odb::sqlite::connection_ptr c (odb_db_->connection ());
  sqlite3 * sdb(c->handle());
  sqlite3_stmt * stmt;
  auto rc = sqlite3_prepare_v2(sdb, "select name from schema_version order by version;", -1, &stmt, NULL);
  if (rc==SQLITE_OK) {
    while(sqlite3_step(stmt) == SQLITE_ROW) {
      std::string n = syd::sqlite3_column_text_string(stmt, 0);
      if (n != "sydCommonDatabase") schema_names.push_back(n);
    }
  }

  // try migrate
  odb::schema_version current_version
    (odb::schema_catalog::current_version (*odb_db_, GetDatabaseSchema()));
  LOG(0) << "Try migrate to version " << GetVersionAsString(current_version) << " (" << current_version << ")";
  try {
    odb::transaction t (odb_db_->begin ());
    int n=schema_names.size()-1;
    for(auto i=0; i<schema_names.size(); i++) {
      auto schema = schema_names[i];
      auto version = current_version;
      for(auto x=0; x<n-i; x++) version = version/0x100;
      LOG(0) << "Migration of schema '" << schema << "' to version " << GetVersionAsString(version);
      odb::schema_catalog::migrate(*odb_db_, version, schema);
    }
    t.commit();
  } catch(std::exception & e) {
    LOG(FATAL) << "Error during schema migration. The error is " << e.what()
               << std::endl
               << "Try to alter the db schema manually with sqlite3 ..."
               << "(good luck !)";
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Copy(std::string new_dbname)
{
  // db file
  std::string dbname = GetFilename();
  fs::copy_file(dbname, new_dbname,fs::copy_option::overwrite_if_exists);
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
void syd::Database::Copy(std::string new_dbname, std::string new_folder)
{
  std::shared_ptr<syd::DatabaseInformation> info;
  try {
    odb::transaction transaction (odb_db_->begin());
    info = std::shared_ptr<syd::DatabaseInformation>(odb_db_->query_one<syd::DatabaseInformation>());
    if (info.get() == 0) {
      LOG(FATAL) << "Error cannot get DatabaseInformation";
    }
    info->folder = new_folder;
    odb_db_->update(*info);
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Exception while set temporary folder in DatabaseInformation");
  }

  // copy
  Copy(new_dbname);
  std::string dbfolder = GetDatabaseAbsoluteFolder();
  syd::copyDir(dbfolder, new_folder);

  try {
    odb::transaction transaction (odb_db_->begin());
    info->folder = relative_folder_;
    odb_db_->update(*info);
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Exception while updating DatabaseInformation");
  }
}
// --------------------------------------------------------------------
