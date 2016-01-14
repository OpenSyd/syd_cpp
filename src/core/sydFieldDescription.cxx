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
#include "sydFieldDescription.h"


// --------------------------------------------------------------------
std::ostream & syd::FieldDescription::Print(std::ostream & os) const
{
  os << name_ << " " << type_;
  return os;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::FieldDescription::GetSQLTableName() const
{
  return sql_table_name_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FieldDescription::SetSQLTableName(std::string name)
{
  sql_table_name_ = name;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FieldDescription::SetName(std::string name, std::string type)
{
  name_ = name;
  type_ = type;
}
// --------------------------------------------------------------------
