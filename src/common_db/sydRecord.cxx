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
#include "sydPrintTable.h"
#include "sydPluginManager.h"


// Define static member
// FIXME 
std::map<std::string, std::vector<std::string>> syd::Record::inherit_sql_tables_map_;

// --------------------------------------------------------------------
syd::Record::Record()
{
  id = 0;
  db_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RecordTraitsBase * syd::Record::traits() const
{
  return syd::RecordTraits<Record>::GetTraits();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Record::GetTableName() const
{
  return traits()->GetTableName();
}
// ----------------------------------------------------


// --------------------------------------------------------------------
bool syd::Record::IsPersistent() const
{
  if (db_ == NULL or id==0) return false;
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Record::ToString() const
{
  return std::to_string(id);
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
void syd::Record::DumpInTable(syd::PrintTable & table) const
{
  std::cout << ToString() << std::endl;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::SetDatabasePointer(odb::callback_event event, odb::database & d) const
{
  auto list = syd::DatabaseManager::GetListOfLoadedDatabases();
  auto search = list.find(&d);
  if (search == list.end()) {
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
  if (db_ == NULL) {
    EXCEPTION("Error in 'GetDatabase', the current record from table'"
              << GetTableName() << "' has no db yet. Use SetDatabasePointer before.");
  }
  return db_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::IsEqual(const syd::Record::pointer r1, const syd::Record::pointer r2)
{
  return (r1->ToString() == r2->ToString());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*
  void syd::Record::SetDefaultFields(std::map<std::string, syd::Record::GetFieldFunction> & map) const
  {
  map["id"] = [](syd::Record::pointer r) { return std::to_string(r->id); };
  map["raw"] = [](syd::Record::pointer r) { return r->ToString(); };
  }
*/
// --------------------------------------------------------------------

