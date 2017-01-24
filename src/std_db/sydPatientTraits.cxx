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
#include "sydPatientTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Patient);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Patient>::
BuildMapOfSortFunctions(CompareFunctionMap & map) const
{
  SetDefaultSortFunctions(map);
  map["name"] = [](pointer a, pointer b) -> bool { return a->name < b->name; };
  auto f = [](pointer a, pointer b) -> bool { return a->study_id < b->study_id; };
  map["study_id"] = f;
  map[""] = f; // make it the default
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::Patient>::
BuildMapOfFieldsFunctions(FieldFunctionMap & map) const
{
  SetDefaultFieldFunctions(map);
  map["name"] = [](pointer a) -> std::string { return a->name; };
  map["dicom_patientid"] = [](pointer a) -> std::string { return a->dicom_patientid; };
  map["sex"] = [](pointer a) -> std::string { return a->sex; };
  map["study_id"] = [](pointer a) -> std::string { return std::to_string(a->study_id); };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> std::string syd::RecordTraits<syd::Patient>::
GetDefaultFields() const
{
  std::string s = "id study_id name weight_in_kg sex dicom_patientid";
  return s;
}
// --------------------------------------------------------------------



template<>
typename syd::FieldType<syd::Patient::pointer>::GenericFunction
syd::FieldType<syd::Patient::pointer>::
BuildGenericFunction(CastFunction f) const
{
  std::cout << "FieldType<RECORD> BuildGenericFunction [default]" << std::endl;
  auto g = [f](RecordPointer p) -> std::string {
    DD("default record to string");
    return f(p)->ToString(); };
  return g;
}


// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::Patient>::
BuildFields(FieldMapType & map) const
{
  std::cout << "SPECIFIC RecordTraits<" << GetTableName() << "> BuildFields(map)" << std::endl;

  InitCommonFields(map);
  {
    auto f = [](pointer p) -> std::string & { return p->name; };
    typedef Field<syd::Patient,std::string> T;
    auto t = new T("name", f);
    map["name"] = std::shared_ptr<T>(t);
  }

  {
    auto f = [](pointer p) -> syd::IdType & { return p->study_id; };
    typedef Field<syd::Patient,syd::IdType> T;
    auto t = new T("study_id", f);
    map["study_id"] = std::shared_ptr<T>(t);
  }

  DD(map.size());
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
template<>
void
syd::FieldType<syd::Patient::pointer>::
Compose(CastFunction f, GenericFunction h)
{
  std::cout << "FieldType<Patient> Compose [default]" << std::endl;
  this->gf = [f, h](syd::Record::pointer p) -> std::string {
    auto r = f(p); // r is Patient
    return h(r); // r should be a record
  };
}
// --------------------------------------------------------------------
