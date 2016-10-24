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
#include "sydFitOptions.h"

// --------------------------------------------------------------------
syd::FitOptions::FitOptions()
{
  r2_min = 0.9;
  max_iteration = 50;
  restricted_tac = false;
  akaike_criterion = "AIC";
  // FIXME additional points
  // FIXME post processes
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::FitOptions::GetModelsName() const
{
  std::stringstream ss;
  for(auto m:model_names) ss << m << " ";
  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::FitOptions::ToString() const
{
  std::stringstream ss;
  ss << GetModelsName() << " "
     << r2_min << " "
     << max_iteration << " "
     << (restricted_tac? "restricted":"non_restricted") << " "
     << akaike_criterion;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOptions::
SetToOptions(syd::TimeIntegratedActivityFitOptions & options) const
{
  DDF();
  options.SetR2MinThreshold(r2_min);
  options.SetMaxNumIterations(max_iteration);
  options.SetRestrictedFlag(restricted_tac);
  options.ClearModels();
  for(auto m:model_names)
    options.AddModel(m);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeIntegratedActivityFitOptions syd::FitOptions::GetOptions() const
{
  syd::TimeIntegratedActivityFitOptions options;
  SetToOptions(options);
  return options;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOptions::
SetFromOptions(const syd::TimeIntegratedActivityFitOptions & options)
{
  DDF();
  r2_min = options.GetR2MinThreshold();
  max_iteration = options.GetMaxNumIterations();
  restricted_tac = options.GetRestrictedFlag();
  model_names.clear();
  for(auto m:options.GetModels())
    model_names.push_back(m->GetName());
}
// --------------------------------------------------------------------
