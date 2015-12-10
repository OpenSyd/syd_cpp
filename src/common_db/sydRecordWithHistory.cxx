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

// --------------------------------------------------------------------
syd::RecordWithHistory::RecordWithHistory():syd::Record()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordWithHistory::InitTable(syd::PrintTable & ta) const
{
  ta.AddFormat("history", "Display the history of the records");
  ta.AddColumn("id"); // FIXME to change for filelist ?

  auto & f = ta.GetFormat();
  if (f == "history") {
    ta.AddColumn("insert");
    ta.AddColumn("update");
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordWithHistory::DumpInTable(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  auto & f = ta.GetFormat();
  if (f == "history") {
    if (history == NULL) {
      LOG(WARNING) << "Error no history ?";
      ta.Set("insert", "NULL");
      ta.Set("update", "NULL");
    }
    ta.Set("insert", history->insertion_date);
    ta.Set("update", history->update_date);
  }
}
// --------------------------------------------------------------------
