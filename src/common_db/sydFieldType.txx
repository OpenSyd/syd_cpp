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

#include <typeinfo>       // operator typeid

// --------------------------------------------------------------------
template<class FieldValueType>
syd::FieldType<FieldValueType>::FieldType(std::string name):FieldBase(name)
{
  this->type = typeid(FieldValueType).name();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class FieldValueType>
syd::FieldType<FieldValueType>::~FieldType()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class FieldValueType>
typename syd::FieldType<FieldValueType>::ToStringFunction
syd::FieldType<FieldValueType>::
BuildToStringFunction(CastFunction f) const
{
  auto g = [f](RecordPointer p) -> std::string {
    auto a = f(p);
    if (a == nullptr) return empty_value;
    return a->ToString();
  };
  return g;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class FieldValueType>
typename syd::FieldType<FieldValueType>::ToStringFunction
syd::FieldType<FieldValueType>::
BuildToStringFunction(ROCastFunction f) const
{
  auto g = [f](RecordPointer p) -> std::string {
    auto a = f(p);
    if (a == nullptr) return empty_value;
    return a->ToString();
  };
  return g;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class FieldValueType>
typename syd::FieldType<FieldValueType>::ToStringFunction
syd::FieldType<FieldValueType>::
BuildComposedFunction(CastFunction f, ToStringFunction h) const
{
  auto gf = [h,f](RecordPointer p) -> std::string {
    auto a = f(p);
    if (a == nullptr) return empty_value;
    return h(a); };
  return gf;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class FieldValueType>
typename syd::FieldType<FieldValueType>::ToStringFunction
syd::FieldType<FieldValueType>::
BuildComposedFunction(ROCastFunction f, ToStringFunction h) const
{
  auto gf = [h,f](RecordPointer p) -> std::string {
    auto a = f(p);
    if (a == nullptr) return empty_value;
    return h(a); };
  return gf;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class FieldValueType>
typename syd::FieldType<FieldValueType>::SortFunction
syd::FieldType<FieldValueType>::
BuildComposedFunction(CastFunction f, SortFunction h) const
{
  auto gf = [h,f](const RecordPointer a, const RecordPointer b) -> bool {
    if (a == nullptr) return true;
    if (b == nullptr) return false;
    auto aa = f(a);
    auto bb = f(b);
    if (aa == nullptr) { return true; }
    if (bb == nullptr) { return false; }
    return h(aa,bb);};
  return gf;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class FieldValueType>
typename syd::FieldType<FieldValueType>::SortFunction
syd::FieldType<FieldValueType>::
BuildComposedFunction(ROCastFunction f, SortFunction h) const
{
  auto gf = [h,f](const RecordPointer a, const RecordPointer b) -> bool {
    if (a == nullptr) return true;
    if (b == nullptr) return false;
    auto aa = f(a);
    auto bb = f(b);
    if (aa == nullptr) return true;
    if (bb == nullptr) return false;
    auto res = h(aa,bb);
    return res;
  };
  return gf;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class FieldValueType>
typename syd::FieldType<FieldValueType>::SortFunction
syd::FieldType<FieldValueType>::
BuildSortFunction(CastFunction cast) const
{
  auto sort = [cast](const RecordPointer a, const RecordPointer b) -> bool {
    auto aa = cast(a);
    auto bb = cast(b);
    return aa < bb; };
  return sort;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class FieldValueType>
typename syd::FieldType<FieldValueType>::SortFunction
syd::FieldType<FieldValueType>::
BuildSortFunction(ROCastFunction cast) const
{
  auto sort = [cast](const RecordPointer a, const RecordPointer b) -> bool {
    auto aa = cast(a);
    auto bb = cast(b);
    return aa < bb; };
  return sort;
}
// --------------------------------------------------------------------
