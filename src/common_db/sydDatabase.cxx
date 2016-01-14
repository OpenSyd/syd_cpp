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
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Database::~Database()
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

  // Define the tables
  CreateTables();

  // Register to the loaded database
  ListOfLoadedDatabases[odb_db_] = this;
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
void syd::Database::Dump(std::ostream & os) const
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
void syd::Database::Dump(const std::string & table_name, const std::string & format, std::ostream & os) const
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
void syd::Database::Insert(generic_record_vector records, const std::string & table_name)
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
void syd::Database::Update(generic_record_vector records, const std::string & table_name)
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
syd::Database::New(const std::string & table_name) const
{
  return GetTable(table_name)->New();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Set(generic_record_pointer record,
                        const std::vector<std::string> & args) const
{
  record->Set(this, args);
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
long syd::Database::GetNumberOfElements(const std::string & table_name) const
{
  return GetTable(table_name)->GetNumberOfElements();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Sort(generic_record_vector & records, const std::string & table_name, const std::string & type) const
{
  if (records.size() == 0) return;
  GetTable(table_name)->Sort(records, type);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Delete(generic_record_vector & records, const std::string & table_name)
{
  if (records.size() == 0) return;
  GetTable(table_name)->Delete(records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DatabaseDescription * syd::Database::GetDatabaseDescription()
{
  if (description_ == NULL) InitDatabaseDescription();
  return description_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::InitDatabaseDescription()
{
  description_ = new DatabaseDescription();
  description_->SetDatabaseName(GetDatabaseSchema());

  for(auto m:GetMapOfTables()) {
    auto & table_name = m.first;
    auto table = m.second;
    table->InitTableDescription(description_);
    description_->AddTableDescription(table->GetTableDescription());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Database::Update(generic_record_pointer record,
                           std::string fieldname,
                           std::string value)
{
  std::string table_name = record->GetTableName();
  auto desc = GetDatabaseDescription();
  auto tdesc = desc->GetTableDescription(table_name);
  syd::FieldDescription * field = tdesc->GetField(fieldname);

  std::string table_sql_name = field->GetSQLTableName();
  std::string field_sql_name = field->GetName();
  std::string v = value;

  std::ostringstream sql;
  sql << "UPDATE \"" << table_sql_name << "\""
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

}
// --------------------------------------------------------------------
