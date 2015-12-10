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
#include "sydRecordHistory.h"
//#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
syd::RecordHistory::RecordHistory():syd::Record()
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
    //<< record->id << " "
     << insertion_date << " "
     << update_date;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::RecordHistory::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and
          //record->IsEqual(p->record) and
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
