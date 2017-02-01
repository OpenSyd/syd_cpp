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


#define DEFINE_TOSTRING_FUNCTION(TYPE)             \
  template<>                                      \
  typename syd::FieldType<TYPE>::ToStringFunction  \
  syd::FieldType<TYPE>::                          \
  BuildToStringFunction(CastFunction f) const

#define DEFINE_TOSTRING_RO_FUNCTION(TYPE)          \
  template<>                                      \
  typename syd::FieldType<TYPE>::ToStringFunction  \
  syd::FieldType<TYPE>::                          \
  BuildToStringFunction(ROCastFunction f) const


// --------------------------------------------------------------------
DEFINE_TOSTRING_FUNCTION(std::string)
{
  return [f](RecordPointer p) -> std::string { return f(p); };
}
DEFINE_TOSTRING_RO_FUNCTION(std::string)
{
  return [f](RecordPointer p) -> std::string { return f(p); };
}
// --------------------------------------------------------------------


// // --------------------------------------------------------------------
// DEFINE_TOSTRING_FUNCTION(syd::FieldBase::RecordPointer)
// {
//   //FIXME NEVER HERE
//   return [f](RecordPointer p) -> std::string {
//     auto a = f(p);
//     if (a == nullptr) return empty_value;
//     return a->ToString();
//   };
// }
// DEFINE_TOSTRING_RO_FUNCTION(syd::FieldBase::RecordPointer)
// {
//   //FIXME NEVER HERE
//   return [f](RecordPointer p) -> std::string {
//     auto a = f(p);
//     if (a == nullptr) return empty_value;
//     return a->ToString();
//   };
// }
// // --------------------------------------------------------------------


// --------------------------------------------------------------------
DEFINE_TOSTRING_FUNCTION(syd::IdType)
{
  return [f](RecordPointer p) -> std::string { return std::to_string(f(p)); };
}
DEFINE_TOSTRING_RO_FUNCTION(syd::IdType)
{
  return [f](RecordPointer p) -> std::string { return std::to_string(f(p)); };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
DEFINE_TOSTRING_FUNCTION(int)
{
  return [f](RecordPointer p) -> std::string { return std::to_string(f(p)); };
}
DEFINE_TOSTRING_RO_FUNCTION(int)
{
  return [f](RecordPointer p) -> std::string { return std::to_string(f(p)); };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
DEFINE_TOSTRING_FUNCTION(unsigned short int)
{
  return [f](RecordPointer p) -> std::string { return std::to_string(f(p)); };
}
DEFINE_TOSTRING_RO_FUNCTION(unsigned short int)
{
  return [f](RecordPointer p) -> std::string { return std::to_string(f(p)); };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
DEFINE_TOSTRING_FUNCTION(double)
{
  auto prec = this->precision;
  auto g = [prec, f](RecordPointer p) -> std::string {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(prec) << f(p);
    return ss.str(); };
  return g;
}
DEFINE_TOSTRING_RO_FUNCTION(double)
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
DEFINE_COMPOSE(syd::IdType);
DEFINE_COMPOSE(double);
DEFINE_COMPOSE(std::string);
DEFINE_COMPOSE(int);
DEFINE_COMPOSE(unsigned short int);
// --------------------------------------------------------------------

