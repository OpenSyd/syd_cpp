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

#include "sydDatabase.h"


// --------------------------------------------------------------------
/// Constructor, set the pointer to the database
template<class RecordType>
syd::Table<RecordType>::Table(syd::Database * d):TableBase()
{
  db_ = d;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename syd::Table<RecordType>::generic_record_pointer
syd::Table<RecordType>::New()
{
  // This is the only place to create a new record.
  auto p = RecordType::New();
  p->SetDatabasePointer(db_);
  return p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Table<RecordType>::Insert(generic_record_pointer record) const
{
  auto p = std::static_pointer_cast<RecordType>(record);
  db_->Insert<RecordType>(p);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Table<RecordType>::Insert(generic_record_vector records) const
{
  //  auto p = std::dynamic_pointer_cast<Record>(record);
  typename RecordType::vector specific_records;
  for(auto r:records) {
    specific_records.push_back(std::dynamic_pointer_cast<RecordType>(r));
  }
  db_->Insert<RecordType>(specific_records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Table<RecordType>::Update(generic_record_pointer record) const
{
  //  auto p = std::dynamic_pointer_cast<Record>(record);
  auto p = std::static_pointer_cast<RecordType>(record);
  db_->Update<RecordType>(p);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Table<RecordType>::Update(generic_record_vector records) const
{
  //  auto p = std::dynamic_pointer_cast<Record>(record);
  typename RecordType::vector specific_records;
  for(auto r:records) {
    specific_records.push_back(std::dynamic_pointer_cast<RecordType>(r));
  }
  db_->Update<RecordType>(specific_records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Table<RecordType>::QueryOne(generic_record_pointer & record, const syd::IdType & id) const
{
  typename RecordType::pointer p;//auto p = std::static_pointer_cast<RecordType>(record);
  db_->QueryOne<RecordType>(p,id);
  record = p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Table<RecordType>::Query(generic_record_vector & records, const std::vector<syd::IdType> & ids) const
{
  typename RecordType::vector specific_records;
  db_->Query<RecordType>(specific_records, ids);
  for(auto r:specific_records) {
    records.push_back(r);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Table<RecordType>::Query(generic_record_vector & records) const
{
  typename RecordType::vector specific_records;
  db_->Query<RecordType>(specific_records);
  for(auto r:specific_records) {
    records.push_back(r);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
long syd::Table<RecordType>::GetNumberOfElements() const
{
  return db_->GetNumberOfElements<RecordType>();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Table<RecordType>::Sort(generic_record_vector & records,
                                  const std::string & type) const
{
  typename RecordType::vector specific_records;
  for(auto r:records) {
    specific_records.push_back(std::dynamic_pointer_cast<RecordType>(r));
  }
  db_->Sort<RecordType>(specific_records, type);
  records.clear();
  for(auto r:specific_records) {
    records.push_back(r);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Table<RecordType>::Sort(vector & records, const std::string & type) const
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Table<RecordType>::Delete(generic_record_vector & records) const
{
  typename RecordType::vector specific_records;
  for(auto r:records) {
    specific_records.push_back(std::dynamic_pointer_cast<RecordType>(r));
  }
  db_->Delete<RecordType>(specific_records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
std::string syd::Table<RecordType>::GetTableName() const
{
  return RecordType::GetStaticTableName();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
std::string syd::Table<RecordType>::GetSQLTableName() const
{
  return RecordType::GetStaticSQLTableName();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
std::vector<std::string> & syd::Table<RecordType>::GetInheritSQLTableNames() const
{
  static bool already_here = false;
  if (!already_here) RecordType::InitInheritance();
  return syd::Record::inherit_sql_tables_map_[RecordType::GetStaticTableName()];
}
// --------------------------------------------------------------------
