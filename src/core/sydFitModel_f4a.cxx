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
#include "sydFitModel_f4a.h"

// --------------------------------------------------------------------
syd::FitModel_f4a::FitModel_f4a():FitModelBase()
{
  name_ = "f4a";
  params_.resize(3);
  id_ = 5;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModel_f4a::ComputeStartingParametersValues(const syd::TimeActivityCurve::pointer tac)
{
  // Select only the end of the curve (min 2 points);
  auto first_index = tac->FindIndexOfMaxValue();
  first_index = std::min(first_index, tac->size()-3);

  // Initialisation
  params_[0] = tac->GetValue(0);
  params_[1] = 0.0;
  params_[2] = -0.8*GetLambdaPhysicHours();

  // Second part of the curve
  /*  Eigen::Vector2d x;
  LogLinearFit(x, tac, first_index, tac->size());
  double c = x(0);
  double d = x(1);
  params_[0] = c;  // x(0) = log c     --> A1 = exp(x(0))
  params_[1] = -GetLambdaPhysicHours()-d;// x(1) = -(l1+lp)  --> l1 = -x(1)-lp
  */
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::FitModel_f4a::SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac)
{
  syd::FitModelBase::SetProblemResidual(problem, tac);

  // Initialisation
  /*
  params_[0] = tac.GetValue(0); // A1
  params_[1] = 0.0;//GetLambdaPhysicHours(); // lambda_1
  params_[2] = 0.0;//GetLambdaPhysicHours()*1.2; // lambda_2
  */

  // need to be created each time
  residuals_.clear();
  for(auto i=0; i<tac.size(); i++) {
    auto r = new ResidualType(tac.GetTime(i), tac.GetValue(i), GetLambdaPhysicHours());
    residuals_.push_back(r);
  }
  // FIXME --> could be templated by CostFctType and param_nb ?
  for(auto i=0; i<tac.size(); i++) {
    problem->AddResidualBlock(new CostFctType(residuals_[i]),
                              NULL, //new ceres::CauchyLoss(robust_scaling_),
                              &params_[0], &params_[1], &params_[2]);
  }

  // Allow A to be negative
  //  problem->SetParameterLowerBound(&params_[0], 0, 0.0);

  // If l1 is negative, means uptake. If too negative, could be continuous uptake.
  problem->SetParameterLowerBound(&params_[1], 0, 0.0);
  problem->SetParameterLowerBound(&params_[2], 0, 0.0);
  problem->SetParameterUpperBound(&params_[2], 0, 10*GetLambdaPhysicHours());
  problem->SetParameterUpperBound(&params_[2], 0, 10*GetLambdaPhysicHours());

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitModelBase * syd::FitModel_f4a::Clone() const
{
  auto * model = new syd::FitModel_f4a;
  model->CopyFrom(this);
  return model;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template <typename T>
bool
syd::FitModel_f4a::ResidualType::operator()(const T* const A1,
                                           const T* const lambda_1,
                                           const T* const lambda_2,
                                           T* residual) const
{
  residual[0] = (T(y_) - (
                          A1[0]*(exp(-(lambda_1[0]+lambda) * T(x_)) -
                                 exp(-(lambda_2[0]+lambda) * T(x_)))
                          ));
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f4a::GetValue(const double & t) const
{
  const double A1 = params_[0];
  const double lambda_1 = params_[1];
  const double lambda_2 = params_[2];
  const double l = lambda_phys_hours_;
  return A1 * (exp(-(l+lambda_1)*t) -
               exp(-(l+lambda_2)*t));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f4a::GetA(const int i) const
{
  if (i==0) return params_[0];
  else return -params_[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f4a::GetLambda(const int i) const
{
  if (i==0) return params_[1];
  else return params_[2];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::FitModel_f4a::IsAcceptable() const
{
  bool is_ok = true;
  for(auto k=0; k<GetNumberOfExpo(); k++) {
    double A = GetA(k);
    double l = GetLambda(k) + GetLambdaPhysicHours();
    //if (l<-0.2*GetLambdaPhysicHours()) is_ok = false;
  }
  return is_ok;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModel_f4a::Scale(double s)
{
  params_[0] *= s;
}
// --------------------------------------------------------------------
