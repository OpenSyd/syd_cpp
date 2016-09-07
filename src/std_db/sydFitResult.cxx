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
  ss << id << " ";
  if (timepoints != NULL) {
    ss << timepoints->patient->name << " "
       << timepoints->injection->radionuclide->name << " "
       << (timepoints->mask == NULL ? "no_mask":timepoints->mask->roitype->name) << " "
       << timepoints->id << " ";
  }
  else ss << "no_tp ";
  ss << GetLabels(tags) << " "
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
void syd::FitResult::DumpInTable(syd::PrintTable & ta) const
{
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
void syd::FitResult::DumpInTable_default(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", timepoints->patient->name);
  ta.Set("m", (timepoints->mask == NULL ? "no_mask":timepoints->mask->roitype->name));
  ta.Set("inj", timepoints->injection->radionuclide->name);
  ta.Set("tp", timepoints->id);
  ta.Set("nb", timepoints->times.size());
  ta.Set("tags", GetLabels(tags));
  ta.Set("model", model_name);
  ta.Set("auc", auc, 7);
  ta.Set("r2", r2, 2);
  ta.Set("index", first_index);
  ta.Set("iter", iterations);
  for(auto i=0; i<params.size(); i++)
    ta.Set("p"+std::to_string(i), params[i], 7);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitResult::DumpInTable_history(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", timepoints->patient->name);
  ta.Set("inj", timepoints->injection->radionuclide->name);
  syd::RecordWithHistory::DumpInTable(ta);
  ta.Set("tp", timepoints->id);
  ta.Set("nb", timepoints->times.size());
  ta.Set("tags", GetLabels(tags));
  ta.Set("model", model_name);
  ta.Set("auc", auc);
  ta.Set("r2", r2);
  ta.Set("i", first_index);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitResult::DumpInTable_md5(syd::PrintTable & ta) const
{

  ta.Set("id", id);
  ta.Set("p", timepoints->patient->name);
  ta.Set("inj", timepoints->injection->radionuclide->name);
  ta.Set("tags", GetLabels(tags));
  ta.Set("tp", timepoints->id);
  ta.Set("nb", timepoints->times.size());
  syd::RecordWithMD5Signature::DumpInTable(ta);
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


// --------------------------------------------------------------------
syd::FitModelBase * syd::FitResult::NewModel()
{
  syd::FitModelBase * model=NULL;
  if (model_name == "f2") model = new syd::FitModel_f2;
  if (model_name == "f3") model = new syd::FitModel_f3;
  if (model_name == "f4a") model = new syd::FitModel_f4a;
  if (model_name == "f4b") model = new syd::FitModel_f4b;
  if (model_name == "f4") model = new syd::FitModel_f4;
  if (model == NULL) {
    LOG(FATAL) << "Model " << model_name << " unknown.";
  }
  model->SetParameters(params);
  model->SetLambdaPhysicHours(timepoints->injection->GetLambdaInHours());
  return model;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitResult::CopyFrom(const syd::FitResult::pointer & from)
{
  // Do not copy id, history, md5
  syd::RecordWithTags::CopyFrom(from);
  timepoints = from->timepoints;
  params = from->params;
  auc = from->auc;
  r2 = from->r2;
  model_name = from->model_name;
  first_index = from->first_index;
  iterations = from->iterations;
}
// --------------------------------------------------------------------
