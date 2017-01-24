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

// syd
#include "sydFieldType.h"
#include "sydRecord.h"

// --------------------------------------------------------------------
template<>
typename syd::FieldType<std::string>::GenericFunction
syd::FieldType<std::string>::
BuildGenericFunction(CastFunction f) const
{
  return [f](RecordPointer p) -> std::string { return f(p); };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
typename syd::FieldType<syd::FieldBase::RecordPointer>::GenericFunction
syd::FieldType<syd::FieldBase::RecordPointer>::
BuildGenericFunction(CastFunction f) const
{
  return [f](RecordPointer p) -> std::string {
    auto a = f(p);
    if (a == nullptr) return empty_value;
    return a->ToString();
  };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
typename syd::FieldType<syd::IdType>::GenericFunction
syd::FieldType<syd::IdType>::
BuildGenericFunction(CastFunction f) const
{
  return [f](RecordPointer p) -> std::string { return std::to_string(f(p)); };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
typename syd::FieldType<double>::GenericFunction
syd::FieldType<double>::
BuildGenericFunction(CastFunction f) const
{
  auto prec = this->precision;
  auto g = [prec, f](RecordPointer p) -> std::string {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(prec) << f(p);
    return ss.str(); };
  return g;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::FieldType<syd::IdType>::Compose(CastFunction f, GenericFunction h) {}
template<> void syd::FieldType<std::string>::Compose(CastFunction f, GenericFunction h) {}
template<> void syd::FieldType<double>::Compose(CastFunction f, GenericFunction h) {}
// --------------------------------------------------------------------

