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
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::RecordWithHistory::Callback(odb::callback_event event,
                                      odb::database & odb,
                                      syd::Database * db) const
{
  if (event == odb::callback_event::pre_persist) {
    db->New<syd::RecordHistory>(history);
    history->insertion_date = syd::Now();
    history->update_date = history->insertion_date;
    odb.persist(history);
  }

  if (event == odb::callback_event::pre_update) {
    if (history->id == 1) { // it means the creation date is unknown
      db->New<syd::RecordHistory>(history);
      odb.persist(history);
    }
    history->update_date = syd::Now();
    odb.update(history);
  }

  // Events in Callback const : persist, update, erase
  // event load can only be here if the non-const version does not exist
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordWithHistory::InitTable(syd::PrintTable & ta) const
{
  ta.AddFormat("history", "Display the history of the records");
  auto & f = ta.GetFormat();
  if (f == "history") {
    ta.AddColumn("inserted");
    ta.AddColumn("updated");
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordWithHistory::DumpInTable(syd::PrintTable & ta) const
{
  auto & f = ta.GetFormat();
  if (f == "history") {
    if (history == NULL) {
      LOG(WARNING) << "Error no history ?";
      ta.Set("inserted", "NULL");
      ta.Set("updated", "NULL");
    }
    ta.Set("inserted", history->insertion_date);
    ta.Set("updated", history->update_date);
  }
}
// --------------------------------------------------------------------
