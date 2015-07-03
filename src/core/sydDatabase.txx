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

// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Dump(const std::vector<std::shared_ptr<RecordType>> & records,
                         const std::string & format,
                         std::ostream & os) const
{
  if (records.size() == 0) return;
  syd::PrintTable ta;
  records[0]->InitPrintTable(this, ta, format);
  if (format != "help") {
    for(auto r:records) r->DumpInTable(this, ta, format);
    ta.Print(os);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Insert(std::shared_ptr<RecordType> record)
{
  try {
    odb::transaction t (db_->begin());
    db_->persist(*record);
    t.commit();
  }
  catch (const odb::exception& e) {
    std::string n="";
    if (record != 0) n = record->GetTableName();
    LOG(FATAL) << "Cannot insert the element: <"
               << record << "> in the table '" << n
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
  if (db_ == NULL) {
    LOG(FATAL) << "Could not AddTable, open a db before";
  }
  std::string tablename = RecordType::GetStaticTableName();
  std::string str = tablename;
  std::transform(str.begin(), str.end(),str.begin(), ::tolower);
  auto it = map_lowercase.find(str);
  if (it != map_lowercase.end()) {
    LOG(FATAL) << "When creating the database, a table with the same name '" << tablename
               << "' already exist.";
  }
  auto * t = new Table<RecordType>(this);
  map[tablename] = t;
  map_lowercase[str] = t;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::QueryOne(std::shared_ptr<RecordType> & record,
                             const odb::query<RecordType> & q) const
{
  try {
    odb::transaction transaction (db_->begin());
    auto r = db_->query_one<RecordType>(q);
    if (r.get() == 0) {
      EXCEPTION("No matching record in QueryOne(q) for the table '" << RecordType::GetStaticTableName()
                << ". Last sql query is: "
                << std::endl << GetLastSQLQuery());
    }
    record = r;
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error in QueryOne(q) for the table '" << record->GetTableName()
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
    odb::transaction transaction (db_->begin());
    record = db_->load<RecordType>(id);
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
    odb::transaction transaction (db_->begin());
    typedef odb::result<RecordType> result;
    result r(db_->query<RecordType>(q));
    for(auto i = r.begin(); i != r.end(); i++) {
      std::shared_ptr<RecordType> s = RecordType::New();
      i.load(*s);
      records.push_back(s);
    }
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error during Query(r, q) for the table '" << RecordType::GetStaticTableName()
              << "'. Last sql query is: "
              << std::endl << GetLastSQLQuery());
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
  odb::query<RecordType> q(odb::query<RecordType>::id == ids[0]);
  for(auto id:ids) {
    q = q or odb::query<RecordType>::id == id;
  }
  Query(records, q);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
long syd::Database::GetNumberOfElements() const
{
  // Brute force. This is inefficient. Should use view and COUNT. // FIXME
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
      if (found == std::string::npos) continue;
    }
    if (found != std::string::npos) output.push_back(r);
  }
}
// --------------------------------------------------------------------
