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
#include "sydFitTimepoints.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
syd::FitTimepoints::FitTimepoints():
  syd::Record(),
  syd::RecordWithTags(),
  syd::RecordWithHistory(),
  syd::RecordWithMD5Signature(),
  syd::FitOptions()
{
  timepoints = nullptr;
  auc = 0.0;
  r2 = 0.0;
  model_name = "";
  first_index = 0;
  iterations = 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::FitTimepoints::ToString() const
{
  std::stringstream ss ;
  ss << id << " ";
  if (timepoints != nullptr) {
    ss << timepoints->patient->name << " "
       << timepoints->id << " ";
  }
  else ss << "no_tp ";
  ss << syd::GetLabels(tags) << " "
     << model_name << " "
     << "auc=" << auc << " "
     << "r2=" << r2 << " "
     << "i=" << first_index << " "
     << "it=" << iterations
     << " |";
  for(auto p:params) ss << " " << p;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitTimepoints::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
  syd::RecordWithMD5Signature::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitTimepoints::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
  syd::RecordWithMD5Signature::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitTimepoints::DumpInTable(syd::PrintTable & ta) const
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
void syd::FitTimepoints::DumpInTable_default(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("tp", timepoints->id);
  ta.Set("p", timepoints->patient->name);
  ta.Set("inj", timepoints->injection->radionuclide->name);
  ta.Set("nb", timepoints->times.size());
  ta.Set("tags", GetLabels(tags));
  ta.Set("model", model_name);
  ta.Set("auc", auc, 7);
  ta.Set("r2", r2, 2);
  ta.Set("index", first_index);
  ta.Set("iter", iterations);
  for(auto i=0; i<params.size(); i++)
    ta.Set("p"+std::to_string(i), params[i], 7);
  ta.Set("R2min", r2_min, 3);
  ta.Set("rest", (restricted_tac? "Y":"N"));
  DD(akaike_criterion);
  ta.Set("Ak", akaike_criterion);
  ta.Set("itm", max_iteration);
  ta.Set("models", GetModelsName()); 
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitTimepoints::DumpInTable_history(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("tp", timepoints->id);
  ta.Set("p", timepoints->patient->name);
  ta.Set("inj", timepoints->injection->radionuclide->name);
  syd::RecordWithHistory::DumpInTable(ta);
  ta.Set("nb", timepoints->times.size());
  ta.Set("tags", GetLabels(tags));
  ta.Set("model", model_name);
  ta.Set("auc", auc);
  ta.Set("r2", r2);
  ta.Set("i", first_index);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitTimepoints::DumpInTable_md5(syd::PrintTable & ta) const
{

  ta.Set("id", id);
  ta.Set("tp", timepoints->id);
  ta.Set("p", timepoints->patient->name);
  ta.Set("inj", timepoints->injection->radionuclide->name);
  ta.Set("tags", GetLabels(tags));
  ta.Set("nb", timepoints->times.size());
  syd::RecordWithMD5Signature::DumpInTable(ta);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::FitTimepoints::ToStringForMD5() const
{
  std::stringstream ss;
  for(auto p:params) ss << std::setprecision(30) << p;
  ss << auc << r2 << model_name << first_index << timepoints->ToStringForMD5() << iterations;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitModelBase::pointer syd::FitTimepoints::NewModel() const
{
  syd::FitModelBase::pointer model=nullptr;
  if (model_name == "f2") model = std::make_shared<syd::FitModel_f2>();
  if (model_name == "f3") model = std::make_shared<syd::FitModel_f3>();
  if (model_name == "f4a") model = std::make_shared<syd::FitModel_f4a>();
  if (model_name == "f4b") model = std::make_shared<syd::FitModel_f4b>();
  if (model_name == "f4") model = std::make_shared<syd::FitModel_f4>();
  if (model == nullptr) {
    LOG(FATAL) << "Model " << model_name << " unknown.";
  }
  model->SetParameters(params);
  model->SetLambdaDecayConstantInHours(timepoints->injection->GetLambdaDecayConstantInHours());
  return model;
}
// --------------------------------------------------------------------


