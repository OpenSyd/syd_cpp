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

//#include "sydRecord.h"
//#include "sydRecordTraitsBase.h"

// Default initialisation
template<class RecordType>
syd::RecordTraitsBase * syd::RecordTraits<RecordType>::
singleton_ = nullptr;

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
typename syd::RecordTraits<RecordType>::generic_record_pointer
syd::RecordTraits<RecordType>::CreateNew(syd::Database * db) const
{
  return syd::RecordTraits<RecordType>::New(db);
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


