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

// --------------------------------------------------------------------
syd::Record::Record()
{
  id = -1;
}
// --------------------------------------------------------------------


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
void syd::Record::InitTable(syd::PrintTable & table) const
{
  table.AddColumn("id");
  table.AddColumn("fields");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::DumpInTable(syd::PrintTable & table) const
{
  table.Set("id", id);
  table.Set("fields", ToString());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::SetDatabasePointer(odb::callback_event event, odb::database & d) const
{
  //DD("SetDatabasePointer " + GetTableName()+" " + syd::ToString(event));
  auto search = syd::Database::ListOfLoadedDatabases.find(&d);
  if (search == syd::Database::ListOfLoadedDatabases.end()) {
    LOG(FATAL) << "Error during callback in an object " << GetTableName()
               << " cannot find the db pointer. Event is " << event;
  }
  db_ = search->second;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::Callback(odb::callback_event event, odb::database & db) const
{
  // Events in Callback const : persist, update, erase
  // event load can only be here if the non-const version does not exist
  // DD("Record::Callback_const "+syd::ToString(event)+" "+GetTableName());
  if (event == odb::callback_event::pre_persist or
      event == odb::callback_event::post_load) SetDatabasePointer(event, db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::Callback(odb::callback_event event, odb::database & db)
{
  // Events in Callback non-const : load
  //DD("Record::Callback non const "+syd::ToString(event)+" "+GetTableName());
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











/*



// --------------------------------------------------------------------
syd::RecordHistory::RecordHistory()//:syd::Record()
{
  DDF();
  insertion_date = "insertion_date_not_set";
  update_date = "update_date_not_set";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RecordHistory::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
    //     << record->id << " "
     << insertion_date << " "
     << update_date;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::RecordHistory::IsEqual(const pointer p) const
{
  return (id == p->id and //syd::Record::IsEqual(p) and
          //    record->IsEqual(p->record) and
          insertion_date == p->insertion_date and
          update_date == p->update_date);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordHistory::InitTable(syd::PrintTable & ta) const
{
  auto & f = ta.GetFormat();
  ta.AddColumn("id");
  //  ta.AddColumn("rid");
  ta.AddColumn("insert");
  ta.AddColumn("update");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordHistory::DumpInTable(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  //  ta.Set("rid", record->id);
  ta.Set("insert", insertion_date);
  ta.Set("update", update_date);
}
// --------------------------------------------------------------------

*/
