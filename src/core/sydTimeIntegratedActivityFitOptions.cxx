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
#include "sydTimeIntegratedActivityFitOptions.h"
#include "sydException.h"
#include "sydCommon.h"

// --------------------------------------------------------------------
syd::TimeIntegratedActivityFitOptions::TimeIntegratedActivityFitOptions()
{
  SetR2MinThreshold(0.9);
  SetRestrictedFlag(false);
  SetMaxNumIterations(100);
  SetAkaikeCriterion("AICc");
  SetLambdaDecayConstantInHours(0.0);
  auto f1  = std::make_shared<syd::FitModel_f1>();
  auto f2  = std::make_shared<syd::FitModel_f2>();
  auto f3  = std::make_shared<syd::FitModel_f3>();
  auto f4a = std::make_shared<syd::FitModel_f4a>();
  auto f4b = std::make_shared<syd::FitModel_f4b>();
  auto f4c = std::make_shared<syd::FitModel_f4c>();
  auto f4  = std::make_shared<syd::FitModel_f4>();
  all_models_.push_back(f1);
  all_models_.push_back(f2);
  all_models_.push_back(f3);
  all_models_.push_back(f4a);
  all_models_.push_back(f4b);
  all_models_.push_back(f4c);
  all_models_.push_back(f4);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeIntegratedActivityFitOptions::~TimeIntegratedActivityFitOptions()
{
  //  for(auto m:all_models_) delete(m);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TimeIntegratedActivityFitOptions::ToString() const
{
  std::stringstream ss;
  ss << GetR2MinThreshold() << " "
     << (GetRestrictedFlag() ? "restricted":"non_restricted") << " "
     << GetMaxNumIterations() << " "
     << GetAkaikeCriterion() << " "
     << GetLambdaDecayConstantInHours() << "h ";
  for(auto & m:model_names_) ss << m << " ";
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFitOptions::AddModel(const std::string & model_name)
{
  model_names_.insert(model_name);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFitOptions::SetAkaikeCriterion(const std::string & criterion_name)
{
  if (criterion_name != "AIC" and criterion_name != "AICc") {
    EXCEPTION("Akaike criterion can only be AIC or AICc, while you set " << criterion_name);
  }
  akaike_criterion_ = criterion_name;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFitOptions::AddTimeValue(double time, double value)
{
  DDF();
  DD("TODO");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TimeIntegratedActivityFitOptions::GetModelsName() const
{
  DDF();
  std::stringstream ss;
  auto models = GetModels();
  for(auto m:models)
    ss << m->GetName() << " ";
  auto s = ss.str();
  DD(s);
  return syd::trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitModelBase::vector syd::TimeIntegratedActivityFitOptions::GetModels() const
{
  syd::FitModelBase::vector models;
  for(auto n:model_names_) {
    auto it = std::find_if(all_models_.begin(), all_models_.end(),
                           [n](const syd::FitModelBase::pointer & m)
                           { return m->GetName() == n; });
    if (it == all_models_.end())
      LOG(WARNING) << "Model " << n << " is not known (ignoring).";
    else models.push_back(*it);
  }
  if (lambda_in_hours_ == 0.0) {
    LOG(FATAL) << "The decay constant (lambda in hours) is not set";
  }
  for(auto m:models)
    m->SetLambdaDecayConstantInHours(lambda_in_hours_);
  return models;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFitOptions::Check() const
{
  if (lambda_in_hours_ == 0.0) {
    LOG(FATAL) << "The decay constant (lambda in hours) is not set";
  }
  if (GetAkaikeCriterion() != "AIC" and GetAkaikeCriterion() != "AICc") {
    LOG(FATAL) << "Akaike criterion '"
               << GetAkaikeCriterion() << "' not known"
               << ". Use AIC or AICc";
  }
}
// --------------------------------------------------------------------
