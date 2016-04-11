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
#include "sydFitResult.h"

// --------------------------------------------------------------------
syd::FitResult::FitResult():
  syd::Record(),
  syd::RecordWithTags(),
  syd::RecordWithHistory()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::FitResult::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << timepoints->id << " "
     << GetLabels(tags) << " "
     << model_name << " "
     << auc << " "
     << r2 << " "
     << first_index << " |";
  for(auto p:params) ss << " " << p;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitResult::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitResult::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitResult::InitTable(syd::PrintTable & ta) const
{
  auto f = ta.GetFormat();

  // Set the columns
  if (f == "default") {
    ta.AddColumn("id");
    ta.AddColumn("tp");
    ta.AddColumn("nb");
    ta.AddColumn("tags");
    ta.AddColumn("model");
    ta.AddColumn("auc");
    ta.AddColumn("r2");
    ta.AddColumn("i");
    for(auto i=0; i<params.size(); i++)
      ta.AddColumn("p"+syd::ToString(i), 1);
  }

  if (f == "history") {
    ta.AddColumn("id");
    syd::RecordWithHistory::InitTable(ta);
    ta.AddColumn("tp");
    ta.AddColumn("tags");
    ta.AddColumn("model");
    ta.AddColumn("auc");
    ta.AddColumn("r2");
    ta.AddColumn("i");
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitResult::DumpInTable(syd::PrintTable & ta) const
{
  syd::RecordWithHistory::DumpInTable(ta);
  auto f = ta.GetFormat();

  if (f == "default") {
    ta.Set("id", id);
    ta.Set("tp", timepoints->id);
    ta.Set("nb", timepoints->times.size());
    ta.Set("tags", GetLabels(tags));
    ta.Set("model", model_name);
    ta.Set("auc", auc);
    ta.Set("r2", r2);
    ta.Set("i", first_index);
    for(auto i=0; i<params.size(); i++)
      ta.Set("p"+syd::ToString(i), params[i]);
  }

  if (f == "history") {
    ta.Set("id", id);
    syd::RecordWithHistory::DumpInTable(ta);
    ta.AddColumn("id");
    syd::RecordWithHistory::InitTable(ta);
    ta.Set("tp", timepoints->id);
    ta.Set("nb", timepoints->times.size());
    ta.Set("tags", GetLabels(tags));
    ta.Set("model", model_name);
    ta.Set("auc", auc);
    ta.Set("r2", r2);
    ta.Set("i", first_index);
  }

}
// --------------------------------------------------------------------
