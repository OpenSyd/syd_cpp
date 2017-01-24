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
  std::cout << "Field<" << t << " " << typeid(FieldValueType).name()
            << "> BuildCastFunction " << std::endl;
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
  DDF();
  auto ta = syd::RecordTraits<RecordType>::GetTraits()->GetTableName();
  std::cout << "Field<" << ta << " " << typeid(FieldValueType).name()
            << "> CreateField " << std::endl;
  auto first_field = this->name;
  DD(first_field);
  DD(field_names);
  std::string s = first_field+field_names;

  // Create a field
  typedef Field<RecordType, FieldValueType> T; // first one
  auto a = new T(s, f); // copy ; f = Image->Injection //FIXME COPY
  auto t = std::shared_ptr<T>(a);
  t->type = this->type;
  DD(t->type);

  std::size_t found = field_names.find_first_of(".");
  DD(found);

  // end ?
  if (field_names == "") {
    DD("simplefield");
    auto cast = BuildCastFunction(t->f); // Cast, h = (record)->Injection
    t->gf = syd::FieldType<FieldValueType>::BuildGenericFunction(cast);
    DD("done");
    return t;
  }
  else {
    auto type = t->type;
    DD(type);
    auto subfield = db->GetField2(type, field_names);
    DD(subfield->name);
    DD(subfield->type);
    DD(this->type);
    //subfield->f  = string f(patient::pointer)
    //subfield->gf = string f(record::pointer)
    DD("sub ok");
    // auto cast = BuildCastFunction(subfield->gf); //
    // t->gf = syd::FieldType<T>::BuildGenericFunction(cast);
    auto f_record = BuildCastFunction(t->f); // Cast, h = (record)->Patient

    //    auto a = std::dynamic_pointer_cast<syd::FieldType<FieldValueType>>(subfield);
    //t->gf = a->BuildGenericFunction(f_record);
    // t->gf = subfield->Compose(f_record);
    //t->gf = this->ComposeGenericFunction(f_record, subfield->gf);
    t->Compose(f_record, subfield->gf);
    /*
      auto a = subfield->gf;
      t->gf = [f_record, a](syd::Record::pointer p) -> std::string {
      auto r = std::dynamic_pointer_cast<FieldValueType>(f_record(p)); // r is Patient
      return a(r); // r should be a record
      };
    */
    //auto a = subfield->BuildCastFunction(f_record);

    return t;
    //LOG(FATAL) << "todo";
    /*
    // Create the final generic function
    auto cast = BuildCastFunction(subfield->gf); //t->f;
    t->gf = syd::FieldType<FieldValueType>::BuildGenericFunction(cast);
    DD("end");
    return t;
    */
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*template<class RecordType, class FieldValueType>
void
syd::Field<RecordType, FieldValueType>::
Compose(GenericFunction a)
{
  DDF();
  auto f_record = BuildCastFunction(f);
  this->gf = [f_record, a](syd::Record::pointer p) -> std::string {
    auto r = f_record(p); // r is Patient
    return a(r); // r should be a record
  };
}
*/
//   auto a = [](syd::Record::pointer p) -> FieldValueType & {
//     std::cout << "From " << typeid(FieldValueType).name()
//     <<  " to ";
//     return 
//   };

// }
// --------------------------------------------------------------------
