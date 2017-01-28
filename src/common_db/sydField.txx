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
  rof = nullptr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
syd::Field<RecordType, FieldValueType>::Field(std::string name, ROFunction ff)
  :FieldType<FieldValueType>(name)
{
  rof = ff;
  f = nullptr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
typename syd::Field<RecordType, FieldValueType>::pointer
syd::Field<RecordType, FieldValueType>::Copy() const
{
  typedef Field<RecordType, FieldValueType> T;
  auto a = new T(this->name, this->f);
  auto t = std::shared_ptr<T>(a);
  t->type = this->type;
  t->read_only = this->read_only;
  t->rof = this->rof;
  t->precision = this->precision;
  t->gf = this->gf;
  return t;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
std::string
syd::Field<RecordType, FieldValueType>::
ToString() const
{
  std::ostringstream oss;
  auto table_name = syd::RecordTraits<RecordType>::GetTraits()->GetTableName();
  oss << table_name << "->" << this->name  << " [" << this->type << "] "
      << (this->read_only ? "read_only":"edit") << " prec=" << this->precision
      << " " 
      << (this->f==nullptr? "f_null":"f_ok") << " "
      << (this->rof==nullptr? "rof_null":"rof_ok") << " "
      << (this->gf==nullptr? "gf_null":"gf_ok");
  return oss.str();
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
void
syd::Field<RecordType, FieldValueType>::
SetPrecision(int p)
{
  DDF();
  this->precision = p;
  DD("TODO");
  //  BuildFunction(db);
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
typename syd::Field<RecordType, FieldValueType>::ROCastFunction
syd::Field<RecordType, FieldValueType>::
BuildCastFunction(ROFunction ff) const
{
  auto t = syd::RecordTraits<RecordType>::GetTraits()->GetTableName();
  auto h = [t,ff](RecordPointer p) -> FieldValueType {
    auto r = std::dynamic_pointer_cast<RecordType>(p);
    if (!r) {
      LOG(FATAL) << "Error while using fct cast "
                 << " I expect " << t << std::endl
                 << " while record is " << p->GetTableName();
    }
    return ff(r);
  };
  return h;
}
// --------------------------------------------------------------------




// // --------------------------------------------------------------------
// template<class RecordType, class FieldValueType>
// typename syd::Field<RecordType, FieldValueType>::pointer
// syd::Field<RecordType, FieldValueType>::
// CreateField(std::string name, Function f, std::string type_name)
// {
//   typedef syd::Field<RecordType,FieldValueType> T;
//   auto t = new T(name, f);
//   t->type = type_name;
//   return std::shared_ptr<T>(t);
// }
// // --------------------------------------------------------------------



// // --------------------------------------------------------------------
// template<class RecordType, class FieldValueType>
// typename syd::Field<RecordType, FieldValueType>::pointer
// syd::Field<RecordType, FieldValueType>::
// CreateField(std::string name, ROFunction f, std::string type_name)
// {
//   typedef syd::Field<RecordType,FieldValueType> T;
//   auto t = new T(name, f);
//   t->type = type_name;
//   return std::shared_ptr<T>(t);
// }
// // --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
void
syd::Field<RecordType, FieldValueType>::
BuildFunction(const syd::Database * db)
{
  std::size_t found = this->name.find_first_of(".");
  std::string first_field;
  std::string field_names="";
  if (found != std::string::npos) {
    first_field = this->name.substr(0,found);
    field_names = this->name.substr(found+1, this->name.size());
  }

  if (!this->read_only) {
    auto cast = BuildCastFunction(f);
    if (field_names == "") {
      this->gf = syd::FieldType<FieldValueType>::BuildGenericFunction(cast);
    }
    else {
      auto subfield = db->NewField(this->type, field_names);
      subfield->precision = this->precision;
      subfield->BuildFunction(db);
      this->gf = this->BuildComposedFunction(cast, subfield->gf); // set the gf
    }
  }
  else {
    auto cast = BuildCastFunction(rof);
    if (field_names == "") {
      this->gf = syd::FieldType<FieldValueType>::BuildGenericFunction(cast);
    }
    else {
      auto subfield = db->NewField(this->type, field_names);
      subfield->precision = this->precision;
      subfield->BuildFunction(db);
      this->gf = this->BuildComposedFunction(cast, subfield->gf); // set the gf
    }
  }
}
// --------------------------------------------------------------------
