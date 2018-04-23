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
#include "sydFitModel_f0.h"

// --------------------------------------------------------------------
syd::FitModel_f0::FitModel_f0():FitModelBase()
{
  name_ = "f0";
  params_.resize(1); // FIXME to replace by AddParameter("A1");
  initialActivity_=1.0;
  id_ = 1;
}
// --------------------------------------------------------------------

void syd::FitModel_f0::ComputeStartingParametersValues(const syd::TimeActivityCurve::pointer tac)
{
  const double l = lambda_in_hours_;
  initialActivity_ = tac->GetValue(0)*exp(lambda_in_hours_*tac->GetTime(0));
}

// --------------------------------------------------------------------
void syd::FitModel_f0::SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac)
{
  syd::FitModelBase::SetProblemResidual(problem, tac);

  // Initialisation
  params_[0] = initialActivity_;

  // need to be created each time
  residuals_.clear(); // FIXME --> allocation could be once for all
  for(auto i=0; i<tac.size(); i++) {
    auto r = new ResidualType(tac.GetTime(i), tac.GetValue(i), GetLambdaDecayConstantInHours());
    residuals_.push_back(r); // fixme no push back, set it
  }
  // FIXME --> could be templated by CostFctType and param_nb ?
  for(auto i=0; i<tac.size(); i++) {
    problem->AddResidualBlock(new CostFctType(residuals_[i]), NULL, &params_[0]);
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitModelBase * syd::FitModel_f0::Clone() const
{
  auto * model = new syd::FitModel_f0;
  model->CopyFrom(this);
  return model;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template <typename T>
bool
syd::FitModel_f0::ResidualType::operator()(const T* const A, T* residual) const
{
  residual[0] = (T(y_) - A[0]*exp(-lambda * T(x_)));
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f0::GetValue(const double & time) const
{
  const double A1 = initialActivity_;
  const double l = lambda_in_hours_;
  return A1 * exp(-l*time);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModel_f0::SetA(double A)
{
  initialActivity_ = A;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f0::GetA(const int i) const
{
  return initialActivity_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f0::GetLambda(const int i) const
{
  return 0.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModel_f0::Scale(double s)
{
  params_[0] *= s;
}
// --------------------------------------------------------------------
