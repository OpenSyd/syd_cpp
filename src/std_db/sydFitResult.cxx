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
  syd::RecordWithHistory(),
  syd::RecordWithMD5Signature()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::FitResult::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << timepoints->patient->name << " "
     << timepoints->injection->radionuclide->name << " "
     << (timepoints->mask == NULL ? "no_mask":timepoints->mask->roitype->name) << " "
     << timepoints->id << " "
     << GetLabels(tags) << " "
     << model_name << " "
     << auc << " "
     << r2 << " "
     << first_index << " "
     << iterations
     << " |";
  for(auto p:params) ss << " " << p;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitResult::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
  syd::RecordWithMD5Signature::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitResult::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
  syd::RecordWithMD5Signature::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitResult::InitTable(syd::PrintTable & ta) const
{
  auto f = ta.GetFormat();

    // Set the columns
  if (f == "default") {
    ta.AddColumn("id");
    ta.AddColumn("p");
    ta.AddColumn("m");
    ta.AddColumn("inj");
    ta.AddColumn("tp");
    ta.AddColumn("nb");
    ta.AddColumn("tags");
    ta.AddColumn("model");
    ta.AddColumn("auc",5);
    ta.AddColumn("r2",5);
    ta.AddColumn("index");
    ta.AddColumn("iter");
    for(auto i=0; i<params.size(); i++)
      ta.AddColumn("p"+syd::ToString(i), 4);
  }

  if (f == "history") {
    ta.AddColumn("id");
    ta.AddColumn("p");
    ta.AddColumn("inj");
    syd::RecordWithHistory::InitTable(ta);
    ta.AddColumn("tp");
    ta.AddColumn("tags");
    ta.AddColumn("model");
    ta.AddColumn("auc",5);
    ta.AddColumn("r2",5);
    ta.AddColumn("i");
  }

  if (f == "md5") {
    ta.AddColumn("id");
    ta.AddColumn("p");
    ta.AddColumn("inj");
    ta.AddColumn("tp");
    ta.AddColumn("nb");
    ta.AddColumn("tags");
    syd::RecordWithMD5Signature::InitTable(ta);
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
    ta.Set("p", timepoints->patient->name);
    ta.Set("m", (timepoints->mask == NULL ? "no_mask":timepoints->mask->roitype->name));
    ta.Set("inj", timepoints->injection->radionuclide->name);
    ta.Set("tp", timepoints->id);
    ta.Set("nb", timepoints->times.size());
    ta.Set("tags", GetLabels(tags));
    ta.Set("model", model_name);
    ta.Set("auc", auc);
    ta.Set("r2", r2);
    ta.Set("index", first_index);
    ta.Set("iter", iterations);

    // Add additional column if the nb of values for this record is larger than the previous
    int nb_col = 12;
    int previous_nb = (ta.GetNumberOfColumns()-nb_col);
    for(auto i=previous_nb; i<params.size(); i++) {
      ta.AddColumn("p"+syd::ToString(i), 1);
    }

    for(auto i=0; i<params.size(); i++)
      ta.Set("p"+syd::ToString(i), params[i]);
  }

  if (f == "history") {
    ta.Set("id", id);
    ta.Set("p", timepoints->patient->name);
    ta.Set("inj", timepoints->injection->radionuclide->name);
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

  if (f == "md5") {
    ta.Set("id", id);
    ta.Set("p", timepoints->patient->name);
    ta.Set("inj", timepoints->injection->radionuclide->name);
    ta.Set("tags", GetLabels(tags));
    ta.Set("tp", timepoints->id);
    ta.Set("nb", timepoints->times.size());
    syd::RecordWithMD5Signature::DumpInTable(ta);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::FitResult::ToStringForMD5() const
{
  std::stringstream ss;
  for(auto p:params) ss << std::setprecision(30) << p;
  ss << auc << r2 << model_name << first_index << timepoints->ToStringForMD5() << iterations;
  return ss.str();
}
// --------------------------------------------------------------------
