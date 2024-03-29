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
#include "sydRecordWithHistory.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
syd::RecordWithHistory::RecordWithHistory()
{
  history = NULL;
  print_history_flag_ = true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordWithHistory::Callback(odb::callback_event event,
                                      odb::database & odb,
                                      syd::Database * db) const
{
  if (event == odb::callback_event::pre_persist) {
    history = db->New<syd::RecordHistory>();
    history->insertion_date = syd::Now();
    history->update_date = history->insertion_date;
    odb.persist(history);
  }

  if (event == odb::callback_event::pre_update) {
    history->update_date = syd::Now();
    odb.update(history);
  }

  if (event == odb::callback_event::pre_erase) {
    if (history != NULL) odb.erase(history);
  }

  // Events in Callback const : persist, update, erase
  // event load can only be here if the non-const version does not exist
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordWithHistory::BuildMapOfSortFunctions(CompareFunctionMap & map)
{
  map["update_date"] =
    [](pointer a, pointer b) -> bool { return a->history->update_date < b->history->update_date; };
  map[""] = map["update_date"];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordWithHistory::BuildMapOfFieldsFunctions(FieldFunctionMap & map)
{
  map["update_date"] = [](pointer a) -> std::string { return a->history->update_date; };
  map["insertion_date"] = [](pointer a) -> std::string { return a->history->insertion_date; };
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordWithHistory::
BuildFields(const syd::Database * db, FieldMapType & map)
{
  DDF();
  /*
  auto fd = [](pointer a) -> std::string { return a->history->update_date; };
  AddField<std::string>(map, "update_date", fd);

  auto fi = [](pointer a) -> std::string { return a->history->insertion_date; };
  AddField<std::string>(map, "insertion_date", fi);
  */
  DD("end");
}
// --------------------------------------------------------------------
