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
#include "sydField.h"

// For boost split string
#include <boost/algorithm/string.hpp>

// Default initialisation
template<class RecordType>
syd::RecordTraits<RecordType> * syd::RecordTraits<RecordType>::singleton_ = nullptr;


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
syd::RecordTraits<RecordType>::
RecordTraits(std::string table_name)
  :RecordTraitsBase(table_name)
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
syd::RecordTraits<RecordType> *
syd::RecordTraits<RecordType>::
GetTraits()
{
  if (singleton_ != nullptr)  return singleton_;
  return GetTraits("ERROR_you_should_set_the_table_name: need macros DEFINE_TABLE_XXX (specialisation of RecordTraits<T>::GetTraits)");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
syd::RecordTraits<RecordType> *
syd::RecordTraits<RecordType>::
GetTraits(std::string table_name)
{
  if (singleton_ != nullptr) return singleton_;
  singleton_ = new RecordTraits<RecordType>(table_name);
  return singleton_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename syd::RecordTraits<RecordType>::pointer
syd::RecordTraits<RecordType>::
New(syd::Database * db)
{
  auto p = pointer(new RecordType);
  p->SetDatabasePointer(db);
  return p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename syd::RecordTraits<RecordType>::RecordBasePointer
syd::RecordTraits<RecordType>::
CreateNew(syd::Database * db) const
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
void syd::RecordTraits<RecordType>::
Sort(RecordBaseVector & records,
     const std::string & type) const
{
  auto specific_records = CastFromVectorOfRecords<RecordType>(records);
  InternalSort(specific_records, type);
  records = ConvertToVectorOfRecords(specific_records);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
const typename syd::RecordTraits<RecordType>::CompareFunctionMap &
syd::RecordTraits<RecordType>::
GetSortFunctionMap() const
{
  return compare_record_fmap_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::
InternalSort(vector & v, std::string type) const
{
  if (0){
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
  // sort
  if (type == "") type = "id";
  if (v.size() == 0) return;
  auto db = v[0]->GetDatabase();
  auto f = NewField(db, type);
  f->BuildFunction(db);
  std::sort(begin(v), end(v), f->sort_f);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  SetDefaultSortFunctions(map);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::
SetDefaultSortFunctions(CompareFunctionMap & map) const
{
  auto f = [](pointer a, pointer b) -> bool { return a->id < b->id; };
  map["id"] = f;
  map[""] = f; // default
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void
syd::RecordTraits<RecordType>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  SetDefaultFieldFunctions(map);
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
template<class RecordType>
void
syd::RecordTraits<RecordType>::
SetDefaultFieldFunctions(FieldFunctionMap & map) const
{
  auto f = [](pointer a) -> std::string { return std::to_string(a->id); };
  map["id"] = f;
  auto f2 = [](pointer a) -> std::string { return a->ToString(); };
  map["raw"] = f2;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::
InitFields() const
{
  if (record_field_fmap_.size() != 0) return; // already done
  if (field_fmap_.size() == 0) BuildMapOfFieldsFunctions(field_fmap_); 
  for(auto m:field_fmap_) {
    auto f = m.second;
    auto gf = [f](syd::Record::pointer gr) -> std::string {
      // cast from generic to specific record. Static = no check !!
      auto r = std::static_pointer_cast<RecordType>(gr);
      return f(r);
    };
    record_field_fmap_[m.first] = gf;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
const typename syd::RecordTraits<RecordType>::FieldFunctionMap &
syd::RecordTraits<RecordType>::
GetFieldMap() const
{
  InitFields();
  return field_fmap_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename syd::RecordTraits<RecordType>::RecordFieldFunc
syd::RecordTraits<RecordType>::
GetFieldOLD(std::string field) const
{
  InitFields();

  // Use map
  auto it = record_field_fmap_.find(field);
  if (it == record_field_fmap_.end()) {
    auto field_not_found = [field](syd::Record::pointer p) -> std::string
      { return field+"_not_found"; };
    return field_not_found;
  }
  else return it->second;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
const typename syd::RecordTraits<RecordType>::RecordFieldFunctionMap &
syd::RecordTraits<RecordType>::
GetRecordFieldMap() const
{
  InitFields();
  return record_field_fmap_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
std::vector<typename syd::RecordTraits<RecordType>::RecordFieldFunc>
syd::RecordTraits<RecordType>::
GetFields(std::string fields) const
{
  std::vector<std::string> words;
  syd::GetWords(words, fields);
  std::vector<RecordFieldFunc> f;
  for(auto & w:words) f.push_back(GetFieldOLD(w));
  return f;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
std::string
syd::RecordTraits<RecordType>::
GetDefaultFields() const
{
  return "raw"; // by default: raw output
}
// --------------------------------------------------------------------



//// above will be removed









// --------------------------------------------------------------------
template<class RecordType>
typename syd::RecordTraits<RecordType>::FieldBasePointer
syd::RecordTraits<RecordType>::
NewField(const syd::Database * db, std::string field_names, std::string abbrev) const
{
  // Decompose the field_names
  auto first_field = field_names;
  std::size_t found = field_names.find_first_of(".");
  if (found != std::string::npos) {
    first_field = field_names.substr(0,found);
    field_names = field_names.substr(found, field_names.size());
  }
  else field_names = "";

  std::string name = first_field;
  std::string short_name = "";
  auto index1 = first_field.find_first_of("[");
  if (index1 != std::string::npos) {
    auto index2 = first_field.find_first_of("]");
    if (index2  == std::string::npos) {
      LOG(FATAL) << "malformed field name, need both '[' and ']'";
    }
    name = first_field.substr(0, index1);
    int l = index2-index1-1;
    short_name = first_field.substr(index1+1,l);
  }

  // Get a copy of the existing field
  auto field = FindField(db, name); // this is a copy

  // abbrev ?
  if (short_name != "") field->abbrev = short_name;

  // Change his name (for complex field, that will be build recursively)
  field->name = field->name+field_names;
  if (abbrev != "") field->abbrev = abbrev;
  // Create the main function
  field->BuildFunction(db);
  return field;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename syd::RecordTraits<RecordType>::FieldBaseVector
syd::RecordTraits<RecordType>::
NewFields(const syd::Database * db, std::string field_names) const
{
  // Split field_names into words
  typename syd::RecordTraits<RecordType>::FieldBaseVector fields;
  std::vector<std::string> words;
  syd::GetWords(words, field_names);

  // For each words, look first if it is a format (several fields)
  // or if it is a simple field.
  auto map = GetFieldFormatsMap(db);
  for(auto w:words) {
    auto it = map.find(w);
    if (it != map.end()) {// find it
      auto fs = NewFields(db, it->second);
      for(auto f:fs) fields.push_back(f);
    }
    else {
      auto f = NewField(db, w);
      fields.push_back(f);
    }
  }
  return fields;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
syd::FieldBase::pointer
syd::RecordTraits<RecordType>::
FindField(const syd::Database * db, std::string field_name) const
{
  auto map = GetFieldsMap(db);
  auto it = map.find(field_name);
  if (it == map.end()) {
    std::ostringstream ss;
    for(auto m:map) ss << m.first << " ";
    for(auto m:field_format_map_) ss << m.first << " ";
    EXCEPTION("Cannot find the field '" << field_name
              << "'. Available fields: " << ss.str());
  }
  auto f = it->second->Copy();
  return f;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
const typename syd::RecordTraits<RecordType>::FieldMapType &
syd::RecordTraits<RecordType>::
GetFieldsMap(const syd::Database * db) const
{
  if (field_map_.size() != 0) return field_map_;
  BuildFields(db);
  return field_map_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
const typename syd::RecordTraits<RecordType>::FieldFormatMapType &
syd::RecordTraits<RecordType>::
GetFieldFormatsMap(const syd::Database * db) const
{
  if (field_format_map_.size() != 0) return field_format_map_;
  BuildFields(db);
  return field_format_map_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void
syd::RecordTraits<RecordType>::
BuildFields(const syd::Database * db) const
{
  // Only the default (this function will be overwritten)
  InitCommonFields();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void
syd::RecordTraits<RecordType>::
InitCommonFields() const
{
  // Add the id (read_only)
  ADD_RO_FIELD(id, syd::IdType);
  // Add the raw version (read only)
  auto f = [](pointer p) -> std::string { return p->ToString(); };
  AddField<std::string>("raw", f);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
template<class FieldValueType>
void
syd::RecordTraits<RecordType>::
AddField(std::string name,
         std::function<FieldValueType & (typename RecordType::pointer p)> f,
         std::string abbrev) const
{
  auto t = Field<RecordType, FieldValueType>::New(name, f, false, abbrev);
  // FIXME Check if already exist ?
  field_map_[name] = t;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
template<class FieldValueType>
void
syd::RecordTraits<RecordType>::
AddField(std::string name,
         std::function<FieldValueType (typename RecordType::pointer p)> f,
         std::string abbrev) const
{
  // true = read_only
  auto t = Field<RecordType, FieldValueType>::New(name, f, true, abbrev);
  // FIXME Check if already exist ?
  field_map_[name] = t;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
template<class RecordType2>
void
syd::RecordTraits<RecordType>::
AddTableField(std::string name,
              std::function<typename RecordType2::pointer (typename RecordType::pointer p)> f,
              std::string abbrev) const
{
  auto t = syd::Field<RecordType, typename RecordType2::pointer>::New(name, f, false, abbrev);
  t->type = syd::RecordTraits<RecordType2>::GetTraits()->GetTableName();
  // FIXME Check if already exist ?
  field_map_[name] = t;
}
// --------------------------------------------------------------------

