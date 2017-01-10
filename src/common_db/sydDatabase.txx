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
#include "sydRecordTraits.h"


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Dump(const std::vector<std::shared_ptr<RecordType>> & records,
                         const std::string & format,
                         std::ostream & os)
{
  syd::PrintTable table;
  table.SetFormat(format);
  //table.SetHeaderFlag(!args_info.noheader_flag);
  table.Build(records.begin(), records.end());
  table.Print(os);
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
    for(auto & record:records) odb_db_->persist(*record);
    t.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Cannot insert " << records.size()
              << " element(s) in the table '"
              << RecordTraits<RecordType>::GetTraits()->GetTableName()
              << "'. The error is: "  << e.what()
              << std::endl << "And last sql query is: "
              << std::endl << GetLastSQLQuery());
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
    EXCEPTION("Cannot update " << records.size()
              << " element(s) in the table '"
              << RecordTraits<RecordType>::GetTraits()->GetTableName()
              << "'. The error is: "  << e.what()
              << std::endl << "And last sql query is: "
              << std::endl << GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::UpdateField(std::shared_ptr<RecordType> & record,
                                std::string field_name,
                                std::string value_name)
{
  RecordBasePointer r = record;
  UpdateField(r, field_name, value_name);
  record = std::static_pointer_cast<RecordType>(r);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::AddTable()
{
  auto traits = syd::RecordTraits<RecordType>::GetTraits();
  auto table_name = traits->GetTableName();
  if (map_of_traits_.find(table_name) != map_of_traits_.end()) {
    LOG(FATAL) << "When creating the database, a table with the same name '" << table_name
               << "' already exist.";
  }
  map_of_traits_[table_name] = traits;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// Query a single record according to query
template<class RecordType>
typename RecordType::pointer
syd::Database::QueryOne(const odb::query<RecordType> & q) const
{
  try {
    odb::transaction transaction (odb_db_->begin());
    typename RecordType::pointer r(odb_db_->query_one<RecordType>(q));
    if (r.get() == 0) {
      EXCEPTION("No matching record in QueryOne(q) for the table '"
                << RecordTraits<RecordType>::GetTraits()->GetTableName()
                << "'. Last sql query is: "
                << std::endl << GetLastSQLQuery());
    }
    transaction.commit();
    return r;
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error in QueryOne(q) for the table '"
              << RecordTraits<RecordType>::GetTraits()->GetTableName()
              << "', cannot find the record. Last sql query is: "
              << std::endl << GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// Query a single record according to the id
template<class RecordType>
typename RecordType::pointer
syd::Database::QueryOne(IdType id) const
{
  try {
    odb::transaction transaction (odb_db_->begin());
    typename RecordType::pointer s;
    s = odb_db_->load<RecordType>(id);
    transaction.commit();
    return s;
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error in QueryOne(id) for the table '"
              << RecordTraits<RecordType>::GetTraits()->GetTableName()
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
    EXCEPTION("Error during Query(r, q) for the table '"
              << RecordTraits<RecordType>::GetTraits()->GetTableName()
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
  for(auto i=0; i<ids.size(); i+=SQLITE_MAX_VARIABLE_NUMBER) {
    auto ibegin = ids.begin()+i;
    auto iend = std::min(ids.begin()+i+SQLITE_MAX_VARIABLE_NUMBER, ids.end());
    odb::query<RecordType> q(odb::query<RecordType>::id.in_range(ibegin, iend));
    Query(records, q);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// template<class RecordType>
// long syd::Database::GetNumberOfElements() const
// {
//   DD("GetNumberOfElements template");

//   // FIXME TO remove
//   // Brute force. This is inefficient. Should use view and count.
//   std::vector<std::shared_ptr<RecordType>> records;
//   Query(records);
//   DD(records.size());
//   return GetNumberOfElements<RecordType>();
// }
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
void syd::Database::Sort(std::vector<std::shared_ptr<RecordType>> & records,
                         const std::string & order) const
{
  if (records.size() == 0) return;
  RecordBaseVector v;
  for(auto & r:records) v.push_back(r);
  syd::RecordTraits<RecordType>::GetTraits()->Sort(this, v, order);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename RecordType::pointer syd::Database::New()
{
  return syd::RecordTraits<RecordType>::New(this);
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
    files_to_delete_.clear();
    EXCEPTION("Error while deleting element "
              << record << " in the table '"
              << RecordTraits<RecordType>::GetTraits()->GetTableName()
              << "', message is: " << e.what()
              << std::endl << "And last sql query is: "
              << std::endl << GetLastSQLQuery());
  }
  // post deletion for files
  DeleteFiles();
}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
template<class RecordType>
void syd::Database::Delete(std::vector<std::shared_ptr<RecordType>> & records)
{
  if (records.size() == 0) return;
  try {
    odb::transaction t (odb_db_->begin());
    for(auto r:records) odb_db_->erase(r);
    t.commit();
  }
  catch (const odb::exception& e) {
    files_to_delete_.clear();
    EXCEPTION("Error while deleting "
              << records.size() << " elements in the table '"
              << RecordTraits<RecordType>::GetTraits()->GetTableName()
              << "', message is: " << e.what()
              << std::endl << "And last sql query is: "
              << std::endl << GetLastSQLQuery());
  }
  // post deletion for files
  DeleteFiles();
}
// ------------------------------------------------------------------------
