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
#include "sydDatabase.h"

// Define static member
std::map<std::string, std::vector<std::string>> syd::Record::inherit_sql_tables_map_;

// --------------------------------------------------------------------
syd::Record::Record()
{
  id = -1;
  db_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::Record::IsPersistent() const
{
  if (db_ == NULL) return false;
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::CheckIfPersistant() const
{
  if (IsPersistent()) return;
  EXCEPTION("This record of table '" << GetTableName()
            << " is not persistent (not in the db)." << std::endl
            << ToString());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::Set(const std::vector<std::string> & args)
{
  LOG(FATAL) << "No function Set(args) for table " << GetTableName()
             << ". Use alternative tool to insert an element.";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::Record::IsEqual(const pointer p) const
{
  return (id == p->id);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::InitTable(syd::PrintTable & table) const
{
  table.AddColumn("id");
  table.AddColumn("fields");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::DumpInTable(syd::PrintTable & table) const
{
  table.Set("fields", ToString());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::SetDatabasePointer(odb::callback_event event, odb::database & d) const
{
  auto search = syd::Database::ListOfLoadedDatabases.find(&d);
  if (search == syd::Database::ListOfLoadedDatabases.end()) {
    LOG(FATAL) << "Error during callback in an object " << GetTableName()
               << " cannot find the db pointer. Event is " << event;
  }
  db_ = search->second;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::SetDatabasePointer(syd::Database * db)
{
  db_ = db;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::Callback(odb::callback_event event, odb::database & db) const
{
  // Events in Callback const : persist, update, erase
  // event load can only be here if the non-const version does not exist
  if (event == odb::callback_event::pre_persist or
      event == odb::callback_event::post_load) SetDatabasePointer(event, db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::Callback(odb::callback_event event, odb::database & db)
{
  // Events in Callback non-const : load
  if (event == odb::callback_event::post_load) SetDatabasePointer(event, db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::Record::Check() const
{
  syd::CheckResult r;
  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Database * syd::Record::GetDatabase() const
{
  if (!IsPersistent()) {
    EXCEPTION("Error in 'GetDatabase', the current record from table'"
              << GetTableName() << "' is not persistant. Insert it in the db before.");
  }
  return db_;
}
// --------------------------------------------------------------------
