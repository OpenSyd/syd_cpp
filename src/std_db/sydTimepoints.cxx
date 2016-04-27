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
#include "sydTimepoints.h"

// --------------------------------------------------------------------
syd::Timepoints::Timepoints():
  syd::Record(),
  syd::RecordWithTags(),
  syd::RecordWithHistory(),
  syd::RecordWithMD5Signature()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Timepoints::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << patient->name << " "
     << injection->radionuclide->name << " "
     << times.size() << " "
     << (mask == NULL ? "no_mask":mask->roitype->name) << " "
     << GetLabels(tags) << " ";
  for(auto i=0; i<times.size(); i++)
    ss << times[i] << " " << values[i] << " ";
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Timepoints::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
  syd::RecordWithMD5Signature::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Timepoints::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
  syd::RecordWithMD5Signature::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Timepoints::InitTable(syd::PrintTable & ta) const
{
  auto f = ta.GetFormat();

  // Set the columns
  if (f == "default") {
    ta.AddColumn("id");
    ta.AddColumn("p");
    ta.AddColumn("inj");
    ta.AddColumn("nb");
    ta.AddColumn("mask");
    ta.AddColumn("img");
    ta.AddColumn("tags");
    for(auto i=0; i<times.size(); i++) {
      ta.AddColumn("t"+syd::ToString(i), 1);
      ta.AddColumn("v"+syd::ToString(i), 10);
      // ta.AddColumn("u"+syd::ToString(i), 4);
    }
  }

  if (f == "history") {
    ta.AddColumn("id");
    ta.AddColumn("p");
    syd::RecordWithHistory::InitTable(ta);
    ta.AddColumn("nb");
    ta.AddColumn("tags");
  }

  if (f == "md5") {
    ta.AddColumn("id");
    ta.AddColumn("p");
    ta.AddColumn("nb");
    ta.AddColumn("tags");
    syd::RecordWithMD5Signature::InitTable(ta);
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Timepoints::DumpInTable(syd::PrintTable & ta) const
{
  syd::RecordWithHistory::DumpInTable(ta);
  auto f = ta.GetFormat();

  if (f == "default") {
    ta.Set("id", id);
    ta.Set("p", patient->name);
    ta.Set("inj", injection->radionuclide->name);
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
    int nb_col = 7;
    int previous_nb = (ta.GetNumberOfColumns()-nb_col)/2.0;
    for(auto i=previous_nb; i<times.size(); i++) {
      ta.AddColumn("t"+syd::ToString(i), 1);
      ta.AddColumn("v"+syd::ToString(i), 10);
      // ta.AddColumn("u"+syd::ToString(i), 4);
    }

    for(auto i=0; i<times.size(); i++)
      ta.Set("t"+syd::ToString(i), times[i]);
    for(auto i=0; i<times.size(); i++)
      ta.Set("v"+syd::ToString(i), values[i]);
    // for(auto i=0; i<times.size(); i++)
    //   ta.Set("u"+syd::ToString(i), std_deviations[i]);
  }

  if (f == "history") {
    ta.Set("id", id);
    ta.Set("p", patient->name);
    syd::RecordWithHistory::DumpInTable(ta);
    ta.Set("tags", GetLabels(tags));
    ta.Set("nb", times.size());
  }

  if (f == "md5") {
    ta.Set("id", id);
    ta.Set("p", patient->name);
    ta.Set("tags", GetLabels(tags));
    ta.Set("nb", times.size());
    syd::RecordWithMD5Signature::DumpInTable(ta);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::Timepoints::Check() const
{
  DD(" FIXME check Timepoints history ");
  syd::CheckResult r;
  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Timepoints::GetTAC(syd::TimeActivityCurve & tac)
{
  tac.clear();
  for(auto i=0; i<times.size(); i++) {
    tac.AddValue(times[i], values[i], std_deviations[i]);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Timepoints::ToStringForMD5() const
{
  std::stringstream ss;
  ss << patient->id
     << injection->id;
  for(auto i=0; i<times.size(); i++) {
    ss << std::setprecision(30)
       << times[i] << values[i] << std_deviations[i];
  }
  return ss.str();
}
// --------------------------------------------------------------------
