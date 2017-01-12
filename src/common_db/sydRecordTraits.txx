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

// Default initialisation
template<class RecordType>
syd::RecordTraitsBase * syd::RecordTraits<RecordType>::
singleton_ = nullptr;


// --------------------------------------------------------------------
template<class RecordType>
std::vector<std::shared_ptr<RecordType>>
syd::CastFromVectorOfRecords(const syd::RecordTraitsBase::RecordBaseVector & records)
{
  std::vector<std::shared_ptr<RecordType>> specific;
  for(auto & r:records)
    specific.push_back(std::static_pointer_cast<RecordType>(r));
  return specific;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
syd::RecordTraitsBase::RecordBaseVector
syd::ConvertToVectorOfRecords(const std::vector<std::shared_ptr<RecordType>> & records)
{
  syd::RecordTraitsBase::RecordBaseVector generic;
  for(auto & r:records) generic.push_back(r);
  return generic;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
syd::RecordTraits<RecordType>::RecordTraits(std::string table_name)
  :RecordTraitsBase(table_name)
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
syd::RecordTraitsBase * syd::RecordTraits<RecordType>::GetTraits()
{
  if (singleton_ != nullptr)  return singleton_;
  return GetTraits("ERROR_you_should_set_the_table_name : need macros DEFINE_TABLE_XXX (specialisation of RecordTraits<T>::GetTraits)");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
syd::RecordTraitsBase * syd::RecordTraits<RecordType>::GetTraits(std::string table_name)
{
  if (singleton_ != nullptr) return singleton_;
  singleton_ = new RecordTraits<RecordType>(table_name);
  return singleton_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename syd::RecordTraits<RecordType>::pointer
syd::RecordTraits<RecordType>::New(syd::Database * db)
{
  auto p = pointer(new RecordType);
  p->SetDatabasePointer(db);
  return p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename syd::RecordTraits<RecordType>::RecordBasePointer
syd::RecordTraits<RecordType>::CreateNew(syd::Database * db) const
{
  return syd::RecordTraits<RecordType>::New(db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename syd::RecordTraits<RecordType>::RecordBasePointer
syd::RecordTraits<RecordType>::
QueryOne(const syd::Database * db, IdType id) const
{
  return db->QueryOne<RecordType>(id);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::
Query(const syd::Database * db,
      RecordBaseVector & records,
      const std::vector<syd::IdType> & ids) const
{
  typename RecordType::vector specific_records;
  db->Query<RecordType>(specific_records, ids);
  for(auto r:specific_records) records.push_back(r);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::
Query(const syd::Database * db,
      RecordBaseVector & records) const
{
  typename RecordType::vector specific_records;
  db->Query<RecordType>(specific_records);
  for(auto r:specific_records) records.push_back(r);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::
Insert(syd::Database * db, RecordBasePointer record) const
{
  auto p = std::static_pointer_cast<RecordType>(record);
  db->Insert<RecordType>(p);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::
Insert(syd::Database * db, const RecordBaseVector & records) const
{
  //  auto p = std::dynamic_pointer_cast<Record>(record);
  typename RecordType::vector specific_records;
  for(auto r:records)
    specific_records.push_back(std::dynamic_pointer_cast<RecordType>(r));
  db->Insert<RecordType>(specific_records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::
Update(syd::Database * db, RecordBasePointer record) const
{
  auto p = std::static_pointer_cast<RecordType>(record);
  db->Update<RecordType>(p);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::
Update(syd::Database * db, const RecordBaseVector & records) const
{
  typename RecordType::vector specific_records;
  for(auto r:records)
    specific_records.push_back(std::dynamic_pointer_cast<RecordType>(r));
  db->Update<RecordType>(specific_records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::
Delete(syd::Database * db, const RecordBaseVector & records) const
{
  typename RecordType::vector specific_records;
  for(auto r:records)
    specific_records.push_back(std::dynamic_pointer_cast<RecordType>(r));
  db->Delete<RecordType>(specific_records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::Sort(RecordBaseVector & records,
                                         const std::string & type) const
{
  DD("RecordTraits<RecordType>::Sort (specific)");
  DD("convert");
  auto specific_records = CastFromVectorOfRecords<RecordType>(records);
  InternalSort(specific_records, type);
  DD("convert back");
  records = ConvertToVectorOfRecords(specific_records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
const typename syd::RecordTraits<RecordType>::CompareFunctionMap &
syd::RecordTraits<RecordType>::GetMapOfSortFunctions() const
{
  return compare_record_fmap_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::InternalSort(vector & v, const std::string & type) const
{
  // Only once: build the map of sorting function
  if (compare_record_fmap_.size() == 0)
    BuildMapOfSortFunctions(compare_record_fmap_);

  // Try to find the sort function
  auto it = compare_record_fmap_.find(type);
  if (it == compare_record_fmap_.end()) {
    std::string help;
    for(auto & c:compare_record_fmap_)
      help += "'"+c.first+"' ";
    LOG(WARNING) << "Cannot find the sorting type '"
                 << type << "'. Current known types are: "
                 << help;
    return;
  }
  std::sort(begin(v), end(v), it->second);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::BuildMapOfSortFunctions(CompareFunctionMap & map)
{
  auto f = [](pointer a, pointer b) -> bool { return a->id < b->id; };
  map["id"] = f;
  map[""] = f; // default 
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*template<class RecordType>
  const typename syd::RecordTraits<RecordType>::FieldFunctionType &
  syd::RecordTraits<RecordType>::GetFieldFunction2(std::string field_name)
  {
  DDF();
  //static std::map<std::string, GetFieldFunction> field_map_;
  static bool already_here = false;
  if (!already_here) BuildFieldFunctionMap();

  auto it = field_map_.find(field_name);
  if (it == field_map_.end()) {
  EXCEPTION("Cannot find the field '" << field_name << "' in table '"
  << GetTableName2() << "'");
  }
  return it->second;
  }
*/
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*template<class RecordType>
  void
  syd::RecordTraits<RecordType>::BuildFieldFunctionMap()
  {
  DDF();
  typedef syd::Record::pointer pointer;
  field_map_["id"] = [](pointer r) { return std::to_string(r->id); };
  field_map_["raw"] = [](pointer r) { return r->ToString(); };
  }*/
// --------------------------------------------------------------------


