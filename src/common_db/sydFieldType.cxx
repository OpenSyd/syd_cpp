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
  std::cout << "FieldType<STRING> BuildGenericFunction [default]" << std::endl;
  auto g = [f](RecordPointer p) -> std::string {
    DD("default no need to convert to_string");
    return f(p); };
  return g;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
typename syd::FieldType<syd::FieldBase::RecordPointer>::GenericFunction
syd::FieldType<syd::FieldBase::RecordPointer>::
BuildGenericFunction(CastFunction f) const
{
  std::cout << "FieldType<RECORD> BuildGenericFunction [default]" << std::endl;
  auto g = [f](RecordPointer p) -> std::string {
    DD("default record to string");
    return f(p)->ToString(); };
  return g;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void
syd::FieldType<syd::FieldBase::RecordPointer>::
Compose(CastFunction f, GenericFunction h)
{
  std::cout << "FieldType<RECORD> Compose [default]" << std::endl;
  this->gf = [f, h](syd::Record::pointer p) -> std::string {
    auto r = f(p); // r is Patient
    return h(r); // r should be a record
  };
}
// --------------------------------------------------------------------
