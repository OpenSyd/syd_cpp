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
typename syd::FieldType<FieldValueType>::GenericFunction
syd::FieldType<FieldValueType>::
BuildGenericFunction(CastFunction f) const
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
typename syd::FieldType<FieldValueType>::GenericFunction
syd::FieldType<FieldValueType>::
BuildGenericFunction(ROCastFunction f) const
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
void
syd::FieldType<FieldValueType>::
Compose(CastFunction f, GenericFunction h)
{
  this->gf = [h,f](RecordPointer p) -> std::string {
    auto a = f(p);
    if (a == nullptr) return empty_value;
    return h(a); };
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<class FieldValueType>
void
syd::FieldType<FieldValueType>::
Compose(ROCastFunction f, GenericFunction h)
{
  this->gf = [h,f](RecordPointer p) -> std::string {
    auto a = f(p);
    if (a == nullptr) return empty_value;
    return h(a); };
}
// --------------------------------------------------------------------

