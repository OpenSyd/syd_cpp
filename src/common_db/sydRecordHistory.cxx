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
  insertion_date = "unknown";
  update_date = "unknown";
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


// --------------------------------------------------
void syd::RecordHistory::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "To insert a Tag, please set <insertion_date> <update_date>";
  }

  insertion_date = arg[0];
  update_date = arg[1];
}
// --------------------------------------------------



// --------------------------------------------------------------------
void syd::RecordHistory::InitTable(syd::PrintTable & ta) const
{
  ta.AddColumn("id"); // FIXME to change for filelist ?
  ta.AddColumn("insertion_date");
  ta.AddColumn("update_date");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordHistory::DumpInTable(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("insertion_date", insertion_date);
  ta.Set("update_date", update_date);
}
// --------------------------------------------------------------------
