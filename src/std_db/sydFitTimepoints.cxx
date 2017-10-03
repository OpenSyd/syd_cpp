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
#include "sydStandardDatabase.h"
#include "sydRecordTraits.h"

DEFINE_TABLE_IMPL(FitTimepoints);

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
     << (unit==nullptr ? empty_value:unit->name) << " "
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
std::string syd::FitTimepoints::ToStringForMD5() const
{
  std::stringstream ss;
  for(auto p:params) ss << std::setprecision(30) << p;
  ss << auc << r2 << GetLabels(tags) << GetModelsName() << first_index
     << timepoints->ToStringForMD5() << GetOptions() << iterations;
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
  if (model_name == "f5") model = std::make_shared<syd::FitModel_f5>();
  if (model == nullptr) {
    LOG(FATAL) << "Model " << model_name << " unknown.";
  }
  model->SetParameters(params);
  model->SetLambdaDecayConstantInHours(timepoints->injection->GetLambdaDecayConstantInHours());
  return model;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeIntegratedActivityFitOptions syd::FitTimepoints::GetOptions() const
{
  auto options = syd::FitOptions::GetOptions();
  options.SetLambdaDecayConstantInHours(timepoints->injection->GetLambdaDecayConstantInHours());
  return options;
}
// --------------------------------------------------------------------

