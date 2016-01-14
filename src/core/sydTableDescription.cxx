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
#include "sydTableDescription.h"


// --------------------------------------------------------------------
std::ostream & syd::TableDescription::Print(std::ostream & os) const
{
  os << table_name_ << " (" << sql_name_
     << ") : " << fields_.size() <<  " fields. ";
  for(auto f:fields_) os << std::endl << "\t" << f;
  return os;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FieldDescription *
syd::TableDescription::GetField(std::string field_name)
{
  auto it = std::find_if(fields_.begin(), fields_.end(),
                         [&field_name](syd::FieldDescription * t)
                         { return t->GetName() == field_name; } );
  if (it == fields_.end()) {
    EXCEPTION("Field '" << field_name << " not found.");
  }
  return *it;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TableDescription::SetTableName(std::string table_name, std::string sql_name)
{
  table_name_ = table_name;
  sql_name_ = sql_name;
  // Check already defined field (inherited)
  for(auto f:fields_) {
    if (f->GetSQLTableName() == "") f->SetSQLTableName(sql_name);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TableDescription::AddField(std::string name, std::string type)
{
  syd::FieldDescription * f = new syd::FieldDescription;
  f->SetSQLTableName(sql_name_);
  f->SetName(name, type);
  fields_.push_back(f);
}
// --------------------------------------------------------------------
