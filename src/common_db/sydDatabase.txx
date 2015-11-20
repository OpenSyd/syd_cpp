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

#include "sydPrintTable.h"


// ------------------------------------------------------------------------
template<class RecordType>
Table<RecordType> * syd::Database::GetTable() const
{
  auto t = GetTable(RecordType::GetStaticTableName());
  return dynamic_cast<Table<RecordType>*>(t);
}
// ------------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Dump(const std::vector<std::shared_ptr<RecordType>> & records,
                         const std::string & format,
                         std::ostream & os) const
{
  if (records.size() == 0) return;
  std::string f = format;

  // Get column width in the format
  std::map<int,int> col_width;
  std::vector<std::string> words;
  syd::GetWords(words, format);
  for(auto w:words) {
    std::size_t found = w.find(":");
    if (found!=std::string::npos) {
      // Get column
      std::string nb = w.substr(0, found);
      int col = atoi(nb.c_str());
      // Get width
      nb = w.substr(found+1, w.size()-found-1);
      int n = atoi(nb.c_str());
      col_width[col] = n;
      // Remove this command from the format
      syd::Replace(f, w, "");
    }
  }
  f = trim(f); // remove spaces

  syd::PrintTable ta;
  records[0]->InitPrintTable(this, ta, f);

  // print
  if (f != "help") {
    // Change col width
    for(auto c:col_width) ta.SetColumnWidth(c.first, c.second);
    // Print rows
    int n=0;
    for(auto r:records) {
      r->DumpInTable(this, ta, f);
      n++;
      if (n != records.size()) ta.Endl(); // no endl at the end
    }
    ta.Print(os);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Insert(std::shared_ptr<RecordType> record)
{
  std::vector<std::shared_ptr<RecordType>> records;
  records.push_back(record);
  Insert(records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Insert(std::vector<std::shared_ptr<RecordType>> records)
{
  try {
    odb::transaction t (odb_db_->begin());
    for(auto record:records) odb_db_->persist(*record);
    t.commit();
  }
  catch (const odb::exception& e) {
    LOG(FATAL) << "Cannot insert " << records.size()
               << " element(s) in the table '" << RecordType::GetStaticTableName()
               << "'. The error is: "  << e.what()
               << std::endl << "And last sql query is: "
               << std::endl << GetLastSQLQuery();
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Update(std::shared_ptr<RecordType> record)
{
  std::vector<std::shared_ptr<RecordType>> records;
  records.push_back(record);
  Update(records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Update(std::vector<std::shared_ptr<RecordType>> records)
{
  try {
    odb::transaction t (odb_db_->begin());
    for(auto record:records) odb_db_->update(*record);
    t.commit();
  }
  catch (const odb::exception& e) {
    LOG(FATAL) << "Cannot update " << records.size()
               << " element(s) in the table '" << RecordType::GetStaticTableName()
               << "'. The error is: "  << e.what()
               << std::endl << "And last sql query is: "
               << std::endl << GetLastSQLQuery();
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::AddTable()
{
  // No exception handling here, fatal error if fail.
  if (odb_db_ == NULL) {
    LOG(FATAL) << "Could not AddTable, open a db before";
  }
  std::string tablename = RecordType::GetStaticTableName();
  std::string str = tablename;
  std::transform(str.begin(), str.end(),str.begin(), ::tolower);
  auto it = map_lowercase_.find(str);
  if (it != map_lowercase_.end()) {
    LOG(FATAL) << "When creating the database, a table with the same name '" << tablename
               << "' already exist.";
  }
  auto * t = new Table<RecordType>(this);
  map_[tablename] = t;
  map_lowercase_[str] = t;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::QueryOne(std::shared_ptr<RecordType> & record,
                             const odb::query<RecordType> & q) const
{
  try {
    odb::transaction transaction (odb_db_->begin());
    typename RecordType::pointer r(odb_db_->query_one<RecordType>(q));
    if (r.get() == 0) {
      EXCEPTION("No matching record in QueryOne(q) for the table '"
                << RecordType::GetStaticTableName()
                << "'. Last sql query is: "
                << std::endl << GetLastSQLQuery());
    }
    record = r;
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error in QueryOne(q) for the table '" << RecordType::GetStaticTableName()
              << "', cannot find the record. Last sql query is: "
              << std::endl << GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::QueryOne(std::shared_ptr<RecordType> & record, const IdType & id) const
{
  try {
    odb::transaction transaction (odb_db_->begin());
    typename RecordType::pointer s;
    s = odb_db_->load<RecordType>(id);
    record = s;
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error in QueryOne sql query for the table '" << RecordType::GetStaticTableName()
              << "' and id = " << id << std::endl
              << "\t odb message: " << e.what() << std::endl
              << "\t last sql query: " << GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Query(std::vector<std::shared_ptr<RecordType>> & records,
                          const odb::query<RecordType> & q) const
{
  try {
    odb::transaction transaction (odb_db_->begin());
    typedef odb::result<RecordType> result;
    result r(odb_db_->query<RecordType>(q));
    for(auto i = r.begin(); i != r.end(); i++) {
      typename RecordType::pointer s;
      s = i.load();
      records.push_back(s);
    }
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error during Query(r, q) for the table '" << RecordType::GetStaticTableName()
              << "'. ODB error is:" << e.what() << std::endl
              << "Last sql query is: " << GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Query(std::vector<std::shared_ptr<RecordType>> & records) const
{
  odb::query<RecordType> q;
  Query(records, q);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Query(std::vector<std::shared_ptr<RecordType>> & records,
                          const std::vector<syd::IdType> & ids) const
{
  if (ids.size() == 0) return;
  odb::query<RecordType> q(odb::query<RecordType>::id.in_range(ids.begin(), ids.end()));
  Query(records, q);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
long syd::Database::GetNumberOfElements() const
{
  // Brute force. This is inefficient. Should use view and count.
  std::vector<std::shared_ptr<RecordType>> records;
  Query(records);
  return records.size();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Grep(std::vector<std::shared_ptr<RecordType>> & output,
                         const std::vector<std::shared_ptr<RecordType>> & input,
                         const std::vector<std::string> & patterns,
                         const std::vector<std::string> & exclude)
{
  // Very slow (for the moment)
  for(auto r:input) {
    std::string s = r->ToString();
    std::size_t found = std::string::npos-1; // found
    for(auto p:patterns) {
      found = s.find(p);
      if (found == std::string::npos) break; // not found
    }
    if (found != std::string::npos) { // still ok, we continue
      found = std::string::npos;
      for(auto e:exclude) {
        found = s.find(e);
        if (found != std::string::npos) break; // found, we stop
      }
      if (found == std::string::npos) output.push_back(r);
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Sort(std::vector<std::shared_ptr<RecordType>> & records, const std::string & order) const
{
  if (records.size() == 0) return;
  auto t = GetTable<RecordType>();
  t->Sort(records, order);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::New(std::shared_ptr<RecordType> & record) const
{
  auto p = GetTable(RecordType::GetStaticTableName())->New();
  record = std::dynamic_pointer_cast<RecordType>(p);
}
// --------------------------------------------------------------------


// ------------------------------------------------------------------------
template<class RecordType>
void syd::Database::Delete(std::shared_ptr<RecordType> record)
{
  try {
    odb::transaction t (odb_db_->begin());
    odb_db_->erase(record);
    t.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error while deleting element "
              << record << " in the table '" << RecordType::GetStaticTableName()
              << "', message is: " << e.what()
              << std::endl << "And last sql query is: "
              << std::endl << GetLastSQLQuery());
  }
}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
template<class RecordType>
void syd::Database::Delete(std::vector<std::shared_ptr<RecordType>> & records)
{
  try {
    odb::transaction t (odb_db_->begin());
    for(auto r:records) odb_db_->erase(r);
    t.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error while deleting "
              << records.size() << " elements in the table '"
              << RecordType::GetStaticTableName()
              << "', message is: " << e.what()
              << std::endl << "And last sql query is: "
              << std::endl << GetLastSQLQuery());
  }
}
// ------------------------------------------------------------------------
