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
  auto g = f;
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
typename syd::Field<RecordType, FieldValueType>::SelfPointer
syd::Field<RecordType, FieldValueType>::
New(std::string name, Function f, bool read_only, std::string abbrev)
{
  auto t = std::make_shared<Self>(name, f);
  t->type = syd::demangle(typeid(FieldValueType).name());
  if (typeid(FieldValueType) == typeid(std::string)) t->type = "string";
  t->read_only = read_only;
  if (abbrev == "") t->abbrev = name;
  else t->abbrev = abbrev;
  return t;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
typename syd::Field<RecordType, FieldValueType>::SelfPointer
syd::Field<RecordType, FieldValueType>::
New(std::string name, ROFunction f, bool read_only, std::string abbrev)
{
  auto t = std::make_shared<Self>(name, f);
  t->type = syd::demangle(typeid(FieldValueType).name());
  if (typeid(FieldValueType) == typeid(std::string)) t->type = "string";
  t->read_only = read_only;
  if (abbrev == "") t->abbrev = name;
  else t->abbrev = abbrev;
  return t;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType, class FieldValueType>
typename syd::Field<RecordType, FieldValueType>::pointer
syd::Field<RecordType, FieldValueType>::Copy() const
{
  auto t = Self::New(this->name, this->f, this->read_only, this->abbrev);
  t->rof = this->rof;
  t->type = this->type;
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
  oss << table_name << "->" << this->name
      << " (" << this->abbrev << ")"
      << " [" << this->type << "] "
      << (this->read_only ? "read_only":"edit") << " prec=" << this->precision
      << " "
      << (this->f==nullptr? "f_null":"f_ok") << " "
      << (this->rof==nullptr? "rof_null":"rof_ok") << " "
      << (this->gf==nullptr? "gf_null":"gf_ok") << " "
      << (this->sort_f==nullptr? "sort_null":"sort_ok");
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
    auto r = std::static_pointer_cast<RecordType>(p);
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
  auto s = this->type;
  auto h = [t,ff,s](RecordPointer p) -> FieldValueType {
    /*DD("cast");
    DD(s);
    DD(syd::RecordTraits<RecordType>::GetTraits()->GetTableName());
    if (p != nullptr) {
      DD(p->GetTableName());
      }*/
    auto r = std::static_pointer_cast<RecordType>(p);
    if (!r) {
      LOG(FATAL) << "Error while using fct cast (ro) "
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

  // The following is *very* ugly (you have been warned).
  if (field_names != "") {
    auto subfield = db->GetField(this->type, field_names);
    subfield->precision = this->precision;
    subfield->BuildFunction(db);
    this->abbrev = subfield->abbrev;
    this->subfield_ = subfield; // need to store to avoid destruction
  }
  else {
    if (this->f != nullptr) {
      auto cast = BuildCastFunction(f);
      this->gf = syd::FieldType<FieldValueType>::BuildToStringFunction(cast);
      this->sort_f = syd::FieldType<FieldValueType>::BuildSortFunction(cast);
      return;
    }
    else {
      auto cast = BuildCastFunction(rof);
      this->gf = syd::FieldType<FieldValueType>::BuildToStringFunction(cast);
      this->sort_f = syd::FieldType<FieldValueType>::BuildSortFunction(cast);
      return;
    }
  }

  // subfield
  if (this->f != nullptr) {
    auto cast = BuildCastFunction(f);
    this->gf = this->BuildComposedFunction(cast, this->subfield_->gf); // set the gf
    this->sort_f = this->BuildComposedFunction(cast, this->subfield_->sort_f); // set the sort_f
  }
  else {
    auto cast = BuildCastFunction(rof);
    this->gf = this->BuildComposedFunction(cast, this->subfield_->gf); // set the gf
    this->sort_f = this->BuildComposedFunction(cast, this->subfield_->sort_f); // set the sort_f
  }
}
// --------------------------------------------------------------------
