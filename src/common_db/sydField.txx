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
  rof = nullptr;// FIXME ff;
  //rof = [f] (typename RecordType::pointer a) -> FieldValueType { return f(a); };
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
  DD(ToString());
  auto a = new T(this->name, this->f);
  auto t = std::shared_ptr<T>(a);
  t->type = this->type;
  t->read_only = this->read_only;
  t->rof = this->rof;
  t->precision = this->precision;
  t->gf = this->gf;
  DD(t->ToString());
  return t;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
std::string
syd::Field<RecordType, FieldValueType>::
ToString() const
{
  DDF();
  std::ostringstream oss;
  auto table_name = syd::RecordTraits<RecordType>::GetTraits()->GetTableName();
  oss << table_name << "->" << this->name  << " [" << this->type << "] "
      << (this->read_only ? "read_only":"edit") << " prec=" << this->precision
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
  //this->precision_ = p; // FIXME no need

  DD(this->read_only);
  DD(this->name);
  /*
    auto first_field = this->name;
    std::size_t found = field_names.find_first_of(".");
    if (found != std::string::npos) {
    first_field = field_names.substr(0,found);
    field_names = field_names.substr(found+1, field_names.size());
    }
    else field_names = "";

    if (!read_only) {
    auto cast = BuildCastFunction(t->f);



    if (field_names == "") {
    t->gf = syd::FieldType<FieldValueType>::BuildGenericFunction(cast);
    t->name = s; // FIXME ??
    return t;
    }
    else {
    auto subfield = db->GetField2(t->type, field_names);
    t->type = subfield->type;
    t->Compose(cast, subfield->gf);
    t->name = s; // FIXME ??
    return t;
    }
    }
    else {
    LOG(FATAL) << "TODO";
    }
  */
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
    std::cout << "Cast from " << p->GetTableName() << " to " << t << std::endl;
    //std::cout << "In field : " << this->ToString() << std::endl; // FIXME no this !!
    // if (p == nullptr) {
    //   LOG(FATAL) << "error p is nullptr";
    // }
    std::cout << "With p : " << p << std::endl;
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
    std::cout << "ROCast from " << p->GetTableName() << " to " << t << std::endl;
    //std::cout << "In field : " << this->ToString() << std::endl; // FIXME no this !!
    std::cout << "With p : " << p << std::endl;
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


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
typename syd::Field<RecordType, FieldValueType>::pointer
syd::Field<RecordType, FieldValueType>::
CreateField(const syd::Database * db, std::string field_names) const
{

  /*

    auto ta = syd::RecordTraits<RecordType>::GetTraits()->GetTableName();
    auto first_field = this->name;
    std::string s = first_field+"."+field_names;
    if (field_names == "") s = first_field;

    // Create a field, change name
    auto t = Copy();
    if (t->gf !=nullptr) {
    DD("no need to build");
    return t;
    }

    // end ?
    if (!t->read_only) {
    auto cast = BuildCastFunction(t->f);
    if (field_names == "") {
    t->gf = syd::FieldType<FieldValueType>::BuildGenericFunction(cast);
    t->name = s; // FIXME ??
    return t;
    }
    else {
    auto subfield = db->GetField2(t->type, field_names);
    t->type = subfield->type;
    t->Compose(cast, subfield->gf);
    t->name = s; // FIXME ??
    return t;
    }
    }
    else {
    auto cast = BuildCastFunction(t->rof); // Cast, h = (record)->Injection
    if (field_names == "") {
    t->gf = syd::FieldType<FieldValueType>::BuildGenericFunction(cast);
    t->name = s; // FIXME ??
    return t;
    }
    else {
    auto subfield = db->GetField2(t->type, field_names);
    t->type = subfield->type;
    t->Compose(cast, subfield->gf);
    t->name = s; // FIXME ??
    return t;
    }
    }
  */
  return nullptr;
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



// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
typename syd::Field<RecordType, FieldValueType>::pointer
syd::Field<RecordType, FieldValueType>::
CreateField(std::string name, ROFunction f, std::string type_name)
{
  typedef syd::Field<RecordType,FieldValueType> T;
  auto t = new T(name, f);
  t->type = type_name;
  return std::shared_ptr<T>(t);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
void
syd::Field<RecordType, FieldValueType>::
BuildFunction(const syd::Database * db, std::string field_names)
{
  DDF();
  DD(ToString());
  //DD(field_names);

  // Name of the field ?
  // 1. "date"          string
  // 2. "patient"       Patient
  // 3. "patient.name"  ? <Patient> ?

  std::size_t found = this->name.find_first_of(".");
  DD(found);
  std::string first_field;
  //  std::string
  field_names="";
  if (found != std::string::npos) {
    first_field = this->name.substr(0,found);
    field_names = this->name.substr(found+1, this->name.size());
  }
  DD(first_field);
  DD(field_names);

  if (!this->read_only) {
    auto cast = BuildCastFunction(f);
    if (field_names == "") {
      DD("simple");
      this->gf = syd::FieldType<FieldValueType>::BuildGenericFunction(cast);
    }
    else {
      DD(this->type);
      auto subfield = db->GetField2(this->type, field_names);
      DD("get subfield");
      DD(subfield);
      subfield->precision = this->precision;
      subfield->BuildFunction(db, "bidon");
      DD(subfield);
      DD("Compose:");
      this->Compose(cast, subfield->gf); // set the gf
      DD(ToString());
      DD("end compose");
    }
  }
  else {
    auto cast = BuildCastFunction(rof);
    if (field_names == "") {
      DD("ro simple");
      this->gf = syd::FieldType<FieldValueType>::BuildGenericFunction(cast);
    }
    else {
      DD(this->type);
      auto subfield = db->GetField2(this->type, field_names);
      DD("ro get subfield");
      DD(subfield);
      subfield->precision = this->precision;
      subfield->BuildFunction(db, "bidon");
      DD(subfield);
      DD("ro Compose:");
      this->Compose(cast, subfield->gf); // set the gf
      DD(ToString());
      DD("ro end compose");
    }
  }
  DD("end Build");
}
// --------------------------------------------------------------------
