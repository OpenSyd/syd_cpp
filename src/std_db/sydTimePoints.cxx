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
#include "sydTimePoints.h"

// --------------------------------------------------------------------
syd::TimePoints::TimePoints():
  syd::RecordWithTags(),
  syd::RecordWithHistory()
{
  DD("TimePoints constructor");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TimePoints::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << times.size() << " "
     << GetLabels(tags) << " ";
  for(auto i=0; i<times.size(); i++)
    ss << times[i] << " " << values[i] << " ";
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimePoints::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimePoints::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimePoints::InitTable(syd::PrintTable & ta) const
{
  syd::RecordWithHistory::InitTable(ta);
  auto f = ta.GetFormat();

  // Set the columns
  if (f == "default" or f == "history") {
    if (ta.GetColumn("id") == -1) ta.AddColumn("id");
    ta.AddColumn("nb");
    ta.AddColumn("tags");
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimePoints::DumpInTable(syd::PrintTable & ta) const
{
  syd::RecordWithHistory::DumpInTable(ta);
  auto f = ta.GetFormat();

  if (f == "default") {
    //    ta.Set("id", id); <--- already done in RecordWithHistory
    ta.Set("tags", GetLabels(tags));
    ta.Set("nb", times.size());
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::TimePoints::Check() const
{
  DD(" FIXME check TimePoints history ");
  syd::CheckResult r;
  return r;
}
// --------------------------------------------------------------------
