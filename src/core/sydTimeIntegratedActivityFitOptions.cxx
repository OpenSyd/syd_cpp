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

// --------------------------------------------------------------------
syd::TimeIntegratedActivityFitOptions::TimeIntegratedActivityFitOptions()
{
  SetR2MinThreshold(0.9);
  SetRestrictedFlag(false);
  SetMaxNumIterations(100);
  SetAkaikeCriterion("AICc");
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
  DDF();
  //  for(auto m:all_models_) delete(m);
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
syd::FitModelBase::vector syd::TimeIntegratedActivityFitOptions::GetModels() const

{
  DDF();
  syd::FitModelBase::vector models;
  for(auto m:all_models_) {
    for(auto n:model_names_) {
      if (n == m->GetName()) {
        DD(n);
        models.push_back(m);
      }
    }
  }
  return models;
}
// --------------------------------------------------------------------
