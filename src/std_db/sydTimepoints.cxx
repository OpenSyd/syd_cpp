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
#include "sydTagHelper.h"

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
     << (patient == NULL ? "no_patient":patient->name) << " "
     << times.size() << " "
     << syd::GetLabels(tags) << " ";
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
void syd::Timepoints::DumpInTable(syd::PrintTable & ta) const
{
  //  syd::RecordWithHistory::DumpInTable(ta);
  auto format = ta.GetFormat();

  if (format == "default") DumpInTable_default(ta);
  else if (format == "history") DumpInTable_history(ta);
  else if (format == "md5") DumpInTable_md5(ta);
  else {
    ta.AddFormat("default", "id, date, tags, size etc");
    ta.AddFormat("history", "with date inserted/updated");
    ta.AddFormat("md5", "with md5");
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Timepoints::DumpInTable_default(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", patient->name);
  ta.Set("tags", GetLabels(tags));
  ta.Set("nb", times.size());
  for(auto i=0; i<times.size(); i++)
    ta.Set("t"+std::to_string(i), times[i], 2);
  for(auto i=0; i<times.size(); i++)
    ta.Set("v"+std::to_string(i), values[i], 6);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Timepoints::DumpInTable_history(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", patient->name);
  syd::RecordWithHistory::DumpInTable(ta);
  ta.Set("tags", GetLabels(tags));
  ta.Set("nb", times.size());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Timepoints::DumpInTable_md5(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", patient->name);
  ta.Set("tags", GetLabels(tags));
  ta.Set("nb", times.size());
  syd::RecordWithMD5Signature::DumpInTable(ta);
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
       << times[i] << values[i];
    if (std_deviations.size() > i) ss << std_deviations[i];
  }
  return ss.str();
}
// --------------------------------------------------------------------


