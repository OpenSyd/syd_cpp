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


template<class RecordType>
syd::RecordTraitsBase * syd::RecordTraits<RecordType>::
singleton_ = nullptr;

// // Default implementation
// template<class RecordType>
// std::string syd::RecordTraits<RecordType>::
// table_name_ = "ERROR_you_should_set_the_table_name";


// --------------------------------------------------------------------
template<class RecordType>
syd::RecordTraits<RecordType>::RecordTraits(std::string table_name)
{
  DDF();
  table_name_ = table_name;
  DD(table_name_);
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<class RecordType>
syd::RecordTraitsBase * syd::RecordTraits<RecordType>::GetTraits(std::string table_name)
{
  DDF();
  if (singleton_ != nullptr) return singleton_;
  DD("First time singleton creation for");
  DD(table_name);
  CreateSingleton(table_name);
  return singleton_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
syd::RecordTraitsBase * syd::RecordTraits<RecordType>::GetTraits()
{
  DDF();
  if (singleton_ != nullptr)  return singleton_;
  DD("First time singleton creation");
  return GetTraits("ERROR_you_should_set_the_table_name");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::RecordTraits<RecordType>::CreateSingleton(std::string table_name)
{
  singleton_ = new RecordTraits<RecordType>(table_name);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
std::string
syd::RecordTraits<RecordType>::GetTableName() const 
{
  return table_name_;
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


