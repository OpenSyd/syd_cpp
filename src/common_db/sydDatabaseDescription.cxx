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
#include "sydDatabase.h"

// --------------------------------------------------------------------
std::ostream& syd::DatabaseDescription::Print(std::ostream & os) const
{
  os << name << ": " << tables_.size() <<  " tables.";
  for(auto t:tables_) os << std::endl << *t;
  return os;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DatabaseDescription::Init(const syd::Database * db)
{
  // Set the schema name
  name = db->GetDatabaseSchema();

  // Step 1: create a first description for all tables. The table name
  // and sql name are set. The field 'id' also.
  for(auto m:db->GetMapOfTables()) {
    auto & table_name = m.first;
    auto table = m.second;
    //    table->InitTableDescription(description_); // init the description
    syd::TableDescription * tdesc = new syd::TableDescription();
    tdesc->SetTableName(table->GetTableName());
    tdesc->SetSQLTableName(table->GetSQLTableName());
    tdesc->AddField("id", "int");
    AddTableDescription(tdesc);
  }

  // Read the db description in the file
  auto sql_description = new syd::DatabaseDescription;
  sql_description->ReadDatabaseSchema(db);

  // Look for fields of type vector ?
  for(auto sqlt:sql_description->GetTablesDescription()) {
    auto fields = sqlt->GetFields();
    if (fields.size() == 3) {
      if ((fields[0]->GetName() == "object_id") and (fields[1]->GetName() == "index")) {
        // In this case, field is a vector
        std::string n = sqlt->GetSQLTableName();
        auto found = n.find("_");
        auto table_name = n.substr(0,found);
        auto field_name = n.substr(found+1, n.size());
        syd::TableDescription * sdt;
        bool b  = FindTableDescriptionFromSQLName(table_name, &sdt);
        if (!b) {
          LOG(FATAL) << "Internal error, cannot find table " << table_name;
        }
        std::string type = "vector_of_"+fields[2]->GetType();
        sdt->AddField(field_name, type);
      }
    }
  }

  // Add the fields to the OO database
  for(auto td:GetTablesDescription()) {
    auto table_name = td->GetTableName();
    auto sql_table_name = td->GetSQLTableName();

    // Find the corresponding sql table
    syd::TableDescription * sdt;
    bool b = sql_description->FindTableDescription(sql_table_name, &sdt);
    if (!b) {
      LOG(FATAL) << "Error, cannot find sql table " << sql_table_name
                 << " in the file (needed for table " << table_name << ")";
    }

    // update the TableDescription with the field of std
    for(auto f:sdt->GetFields()) {
      if (f->GetName() == "id") continue;     // already st by InitTableDescription
      if (f->GetName() == "typeid") continue; // not useful for user
      td->AddField(f);
    }

    // If inherit
    auto table = db->GetMapOfTables().at(table_name);
    // (.at because [table_name] is non const)
    for(auto h:table->GetInheritSQLTableNames()) {
      if (h != "syd::Record") {
        FindTableDescriptionFromSQLName(h, &sdt);
        for(auto f:sdt->GetFields()) {
          if (f->GetName() == "id") continue;     // already st by InitTableDescription
          if (f->GetName() == "typeid") continue; // not useful for user
          td->AddField(f);
        }
      }
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DatabaseDescription::ReadDatabaseSchema(const syd::Database * db)
{
  // get the list of sql tables
  std::vector<std::string> table_names;
  sqlite3 * sdb = db->GetSqliteHandle();
  sqlite3_stmt * stmt;
  auto rc = sqlite3_prepare_v2(sdb, "select * from SQLITE_MASTER", -1, &stmt, NULL);
  if (rc==SQLITE_OK) {
    /* Loop on result with the following structure:
       TABLE sqlite_master
       type TEXT, name TEXT, tbl_name TEXT, rootpage INTEGER, sql TEXT  */
    while(sqlite3_step(stmt) == SQLITE_ROW) {
      std::string type = sqlite3_column_text_string(stmt, 0);
      if (type == "table") {
        std::string table_name = sqlite3_column_text_string(stmt, 2);
        table_names.push_back(table_name);
      }
    }
  }
  else {
    EXCEPTION("Could not retrieve the list of tables in the db");
  }

  // Read table description
  for(auto table_name:table_names) {
    auto ta = new syd::TableDescription;
    ta->ReadTableSchema(db, table_name);
    AddTableDescription(ta);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::DatabaseDescription::FindTableDescription(std::string table_name,
                                                    syd::TableDescription ** d)
{
  std::string tn = ToLowerCase(table_name);
  auto it = std::find_if(tables_.begin(), tables_.end(),
                         [&tn](syd::TableDescription * t)
                         { return ToLowerCase(t->GetTableName()) == tn; } );
  if (it == tables_.end()) return false;
  *d = *it;
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::DatabaseDescription::FindTableDescriptionFromSQLName(std::string table_name,
                                                               syd::TableDescription ** d)
{
  auto it = std::find_if(tables_.begin(), tables_.end(),
                         [&table_name](syd::TableDescription * t)
                         { return t->GetSQLTableName() == table_name; } );
  if (it == tables_.end()) return false;
  *d = *it;
  return true;
}
// --------------------------------------------------------------------
