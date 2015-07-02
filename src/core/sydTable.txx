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
template<class RecordType>
typename syd::Table<RecordType>::generic_record_pointer
syd::Table<RecordType>::New() const
{
  return RecordType::New();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Table<RecordType>::Insert(generic_record_pointer record) const
{
  //  auto p = std::dynamic_pointer_cast<Record>(record);
  auto p = std::static_pointer_cast<RecordType>(record);
  db_->Insert<RecordType>(p);
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
