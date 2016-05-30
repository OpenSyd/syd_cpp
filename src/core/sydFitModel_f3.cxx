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
#include "sydFitModel_f3.h"

// --------------------------------------------------------------------
syd::FitModel_f3::FitModel_f3():FitModelBase()
{
  name_ = "f3";
  params_.resize(3);
  id_ = 3;
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::FitModel_f3::ComputeStartingParametersValues(const syd::TimeActivityCurve::pointer tac)
{
  // Select only the end of the curve (min 2 points);
  auto first_index = tac->FindIndexOfMaxValue();
  first_index = std::min(first_index, tac->size()-3);

  // Initialisation
  params_[0] = 0.0;
  params_[1] = GetLambdaPhysicHours();
  params_[2] = 0.0;

  // log linear fit, simple mean to estimate the A2 parameters
  int n = tac->size()-first_index;
  double b = 0.0;
  for(auto i=first_index; i<tac->size(); i++) {
    b += log(tac->GetValue(i)) - (-GetLambdaPhysicHours() * tac->GetTime(i));
  }
  b /= n;
  params_[2] = exp(b);
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::FitModel_f3::SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac)
{
  syd::FitModelBase::SetProblemResidual(problem, tac);

  // need to be created each time
  residuals_.clear();
  for(auto i=0; i<tac.size(); i++) {
    auto r = new ResidualType(tac.GetTime(i), tac.GetValue(i), GetLambdaPhysicHours());
    residuals_.push_back(r);
  }
  // could be templated by CostFctType and param_nb ? not easy
  for(auto i=0; i<tac.size(); i++) {
    problem->AddResidualBlock(new CostFctType(residuals_[i]),
                              NULL,//new ceres::HuberLoss(robust_scaling_),
                              &params_[0], &params_[1], &params_[2]);
  }

  //problem->SetParameterLowerBound(&params_[0], 0, 0.0);
  problem->SetParameterLowerBound(&params_[1], 0, 0);
  problem->SetParameterUpperBound(&params_[1], 0, 10*GetLambdaPhysicHours());

  /*
  problem->SetParameterLowerBound(&params_[0], 0, 0.0);
  problem->SetParameterLowerBound(&params_[2], 0, 0.0);
  */
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitModelBase * syd::FitModel_f3::Clone() const
{
  auto * model = new syd::FitModel_f3;
  model->CopyFrom(this);
  return model;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template <typename T>
bool
syd::FitModel_f3::ResidualType::operator()(const T* const A1,
                                           const T* const lambda_1,
                                           const T* const A2,
                                           T* residual) const
{
  residual[0] = (T(y_) - (
                          A1[0]*exp(-(lambda_1[0]+lambda) * T(x_)) +
                          A2[0]*exp(-lambda * T(x_))
                          ));
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f3::GetValue(const double & t) const
{
  const double A1 = params_[0];
  const double lambda_1 = params_[1];
  const double A2 = params_[2];
  const double l = lambda_phys_hours_;
  return A1 * exp(-(l+lambda_1)*t) + A2* exp(-l*t);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f3::GetA(const int i) const
{
  if (i==0) return params_[0];
  else return params_[2];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f3::GetLambda(const int i) const
{
  if (i==0) return params_[1];
  else return 0.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModel_f3::Scale(double s)
{
  params_[0] *= s;
  params_[2] *= s;
}
// --------------------------------------------------------------------
