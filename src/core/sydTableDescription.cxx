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
  std::ostringstream oss;
  //if (GetInheritSQLTableNames()[0] != "syd::Record") oss << "[inherit from " << GetInheritSQLTableNames()[0] << "] ";
  os << table_name_ << " (" << sql_table_name_
     << ") : " << oss.str()
     << fields_.size() <<  " fields. ";
  for(auto f:fields_) os << std::endl << "\t" << f;
  return os;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::TableDescription::FindField(std::string field_name, syd::FieldDescription ** f)
{
  auto it = std::find_if(fields_.begin(), fields_.end(),
                         [&field_name](syd::FieldDescription * t)
                         { return t->GetName() == field_name; } );
  if (it == fields_.end()) return false;
  *f = *it;
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TableDescription::AddField(std::string name, std::string type)
{
  syd::FieldDescription * f = new syd::FieldDescription(this);
  //f->SetSQLTableName(sql_name_);
  f->SetName(name, type);
  fields_.push_back(f);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TableDescription::AddField(const syd::FieldDescription * ff)
{
  syd::FieldDescription * f = new syd::FieldDescription(this);
  //f->SetSQLTableName(sql_name_);
  f->SetName(ff->GetName(), ff->GetType());
  fields_.push_back(f);
}
// --------------------------------------------------------------------
