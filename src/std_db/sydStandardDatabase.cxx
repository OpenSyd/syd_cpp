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
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
void syd::StandardDatabase::CreateTables()
{
  AddTable<syd::Patient>();
  AddTable<syd::Injection>();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Patient::pointer syd::StandardDatabase::FindPatient(const std::string & name_or_study_id)
{
  DD("FindPatient");
  syd::Patient::pointer patient;
  //  try{
  DD(name_or_study_id);
  odb::query<syd::Patient> q = odb::query<syd::Patient>::name == name_or_study_id;
  DD("before QueryOne");
  QueryOne(patient, q);//odb::query<syd::Patient>::name == name_or_study_id);
  // }
  // catch (const odb::exception& e) {
  //   LOG(FATAL) << "Error TODO" // FIXME
  //              << e->what()
  //              << std::endl << GetLastSQLQuery();
  // }
  DD(patient);
  return patient;
  //  return NewPatient();
}
// --------------------------------------------------------------------
