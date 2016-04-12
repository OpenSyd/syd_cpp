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
  syd::Record(),
  syd::RecordWithTags(),
  syd::RecordWithHistory()
{
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
  auto f = ta.GetFormat();
  DD(f);
  DD(*this);

  // Set the columns
  if (f == "default") {
    ta.AddColumn("id");
    ta.AddColumn("nb");
    ta.AddColumn("mask");
    ta.AddColumn("img");
    ta.AddColumn("tags");
    for(auto i=0; i<times.size(); i++)
      ta.AddColumn("t"+syd::ToString(i), 1);
    for(auto i=0; i<times.size(); i++)
      ta.AddColumn("v"+syd::ToString(i), 10);
  }

  if (f == "history") {
    ta.AddColumn("id");
    syd::RecordWithHistory::InitTable(ta);
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
  DD(*this);

  if (f == "default") {
    ta.Set("id", id);
    ta.Set("tags", GetLabels(tags));
    if (mask != NULL) ta.Set("mask", mask->roitype->name);
    else ta.Set("mask", "no_mask");
    if (images.size() > 0) {
      std::string s;
      for(auto i:images) s += syd::ToString(i->id)+",";
      s.pop_back();
      ta.Set("img", s);
    }
    else ta.Set("img", "no_img");
    ta.Set("nb", times.size());

    // Add additional column if the nb of values is larger
    int nb_col = 5;
    int previous_nb = (ta.GetNumberOfColumns()-nb_col)/2.0;
    for(auto i=previous_nb; i<times.size(); i++) {
      ta.AddColumn("t"+syd::ToString(i), 1);
      ta.AddColumn("v"+syd::ToString(i), 1);
    }

    for(auto i=0; i<times.size(); i++)
      ta.Set("t"+syd::ToString(i), times[i]);
    for(auto i=0; i<times.size(); i++)
      ta.Set("v"+syd::ToString(i), values[i]);
  }

  if (f == "history") {
    ta.Set("id", id);
    syd::RecordWithHistory::DumpInTable(ta);
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


// --------------------------------------------------------------------
void syd::TimePoints::GetTAC(syd::TimeActivityCurve & tac)
{
  tac.clear();
  for(auto i=0; i<times.size(); i++) {
    tac.AddValue(times[i], values[i], std_deviations[i]);
  }
}
// --------------------------------------------------------------------
