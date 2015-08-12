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
#include "sydFitModel_f1.h"

// --------------------------------------------------------------------
syd::FitModel_f1::FitModel_f1():FitModelBase()
{
  name_ = "f1";
  params_.resize(1); // FIXME to replace by AddParameter("A1");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModel_f1::SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac)
{
  syd::FitModelBase::SetProblemResidual(problem, tac);

  // Initialisation
  params_[0] = tac.GetValue(0);

  // need to be created each time
  residuals_.clear(); // FIXME --> allocation could be once for all
  for(auto i=0; i<tac.size(); i++) {
    auto r = new ResidualType(tac.GetTime(i), tac.GetValue(i), GetLambdaPhysicHours());
    residuals_.push_back(r); // fixme no push back, set it
  }
  // FIXME --> could be templated by CostFctType and param_nb ?
  for(auto i=0; i<tac.size(); i++) {
    problem->AddResidualBlock(new CostFctType(residuals_[i]), NULL, &params_[0]);
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitModelBase * syd::FitModel_f1::Clone() const
{
  auto * model = new syd::FitModel_f1;
  model->CopyFrom(this);
  return model;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template <typename T>
bool
syd::FitModel_f1::ResidualType::operator()(const T* const A, T* residual) const
{
  residual[0] = (T(y_) - A[0]*exp(-lambda * T(x_)));
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f1::GetValue(const double & time) const
{
  const double A1 = params_[0];
  const double l = lambda_phys_hours_;
  return A1 * exp(-l*time);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f1::GetA(const int i) const
{
  return params_[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f1::GetLambda(const int i) const
{
  return 0.0;
}
// --------------------------------------------------------------------
