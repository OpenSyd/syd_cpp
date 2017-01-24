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

// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
syd::Field<RecordType, FieldValueType>::Field(std::string name, Function ff)
  :FieldType<FieldValueType>(name)
{
  f = ff;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
syd::Field<RecordType, FieldValueType>::~Field()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
typename syd::Field<RecordType, FieldValueType>::CastFunction
syd::Field<RecordType, FieldValueType>::
BuildCastFunction(Function f) const
{
  auto t = syd::RecordTraits<RecordType>::GetTraits()->GetTableName();
  auto h = [t,f](RecordPointer p) -> FieldValueType & {
    auto r = std::dynamic_pointer_cast<RecordType>(p);
    if (!r) {
      LOG(FATAL) << "Error while using fct cast "
                 << " I expect " << t << std::endl
                 << " while record is " << p->GetTableName();
    }
    return f(r);
  };
  return h;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
typename syd::Field<RecordType, FieldValueType>::pointer
syd::Field<RecordType, FieldValueType>::
CreateField(const syd::Database * db, std::string field_names) const
{
  auto ta = syd::RecordTraits<RecordType>::GetTraits()->GetTableName();
  auto first_field = this->name;
  std::string s = first_field+"."+field_names;

  // Create a field
  typedef Field<RecordType, FieldValueType> T; // first one
  auto a = new T(s, f); // copy ; f = Image->Injection //FIXME COPY
  auto t = std::shared_ptr<T>(a);
  t->type = this->type;

  // end ?
  if (field_names == "") {
    auto cast = BuildCastFunction(t->f); // Cast, h = (record)->Injection
    t->gf = syd::FieldType<FieldValueType>::BuildGenericFunction(cast);
    return t;
  }
  else {
    auto type = t->type;
    auto subfield = db->GetField2(type, field_names);
    auto f_record = BuildCastFunction(t->f); // Cast, h = (record)->Patient
    t->Compose(f_record, subfield->gf);
    return t;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
typename syd::Field<RecordType, FieldValueType>::pointer
syd::Field<RecordType, FieldValueType>::
CreateField(std::string name, Function f, std::string type_name)
{
  typedef syd::Field<RecordType,FieldValueType> T;
  auto t = new T(name, f);
  t->type = type_name;
  return std::shared_ptr<T>(t);
}
// --------------------------------------------------------------------

