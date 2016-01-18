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
#include "sydDatabaseDescription.h"


// --------------------------------------------------------------------
std::ostream& syd::DatabaseDescription::Print(std::ostream & os) const
{
  os << name << ": " << tables_.size() <<  " tables.";
  for(auto t:tables_) os << std::endl << *t;
  return os;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::DatabaseDescription::FindTableDescription(std::string table_name,
                                                    syd::TableDescription ** d)
{
  auto it = std::find_if(tables_.begin(), tables_.end(),
                         [&table_name](syd::TableDescription * t)
                         { return t->GetTableName() == table_name; } );
  if (it == tables_.end()) return false;
  *d = *it;
  return true;
}
// --------------------------------------------------------------------
