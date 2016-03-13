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
#include "sydDatabase.h"

// --------------------------------------------------------------------
std::ostream & syd::TableDescription::Print(std::ostream & os) const
{
  std::ostringstream oss;
  os << table_name_ << " (" << sql_table_name_
     << ") : " << oss.str()
     << fields_.size() <<  " fields. ";
  for(auto f:fields_) os << std::endl << "\t" << f;
  os << std::endl;
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
  f->SetName(name, type);
  fields_.push_back(f);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TableDescription::AddField(const syd::FieldDescription * ff)
{
  syd::FieldDescription * f = new syd::FieldDescription(this);
  f->SetName(ff->GetName(), ff->GetType());
  fields_.push_back(f);
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::TableDescription::ReadTableSchema(const syd::Database * db,
                                            std::string table_name)
{
  SetTableName(table_name);
  SetSQLTableName(table_name);

  sqlite3 * sdb = db->GetSqliteHandle();
  sqlite3_stmt * stmt;
  std::string q = "PRAGMA table_info("+table_name+")";
  auto rc = sqlite3_prepare_v2(sdb, q.c_str(), -1, &stmt, NULL);
  if (rc==SQLITE_OK) {
    /* Loop on result with the following structure:
       cid name type notnull dflt_value  pk */
    while(sqlite3_step(stmt) == SQLITE_ROW) {
      std::string name = sqlite3_column_text_string(stmt, 1);
      std::string type = sqlite3_column_text_string(stmt, 2);
      AddField(name, type);
    }
  }
  else {
    EXCEPTION("Could not retrieve the list of tables in the db");
  }
}
// --------------------------------------------------------------------
