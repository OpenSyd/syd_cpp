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
#include "sydRecord.h"

// --------------------------------------------------------------------
void syd::Record::Set(const syd::Database * db, const std::vector<std::string> & args)
{
  LOG(FATAL) << "The function Set(db, args) must be implemented for table " << GetTableName();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::Record::IsEqual(const pointer p) const
{
  return (id == p->id);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// void syd::Record::Dump(const syd::Database * db, std::ostream & os, vector records)
// {
//   DD(" dump records");
//   for(auto r:records) {
//     os << r->ToString() << std::endl;
//   }
// }
// --------------------------------------------------------------------


void syd::Record::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  DD(" initprinttable");
}


void syd::Record::DumpInTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  DD(" DumpInTable");
  DD(ToString());
}
