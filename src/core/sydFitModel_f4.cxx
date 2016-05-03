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
#include "sydFitModel_f4.h"

// --------------------------------------------------------------------
syd::FitModel_f4::FitModel_f4():FitModelBase()
{
  name_ = "f4";
  params_.resize(4);
  id_ = 4;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModel_f4::SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac)
{
  syd::FitModelBase::SetProblemResidual(problem, tac);

  // Initialisation
  params_[0] = tac.GetValue(0); // A1
  params_[1] = GetLambdaPhysicHours(); // l1
  params_[2] = -tac.GetValue(0); // A2 start away from A1
  params_[3] = 0.0;//-GetLambdaPhysicHours()/2.0; // l2

  // need to be created each time
  residuals_.clear();
  for(auto i=0; i<tac.size(); i++) {
    auto r = new ResidualType(tac.GetTime(i), tac.GetValue(i), GetLambdaPhysicHours());
    residuals_.push_back(r);
  }
  // FIXME --> could be templated by CostFctType and param_nb ?
  for(auto i=0; i<tac.size(); i++) {
    problem->AddResidualBlock(new CostFctType(residuals_[i]), NULL, &params_[0], &params_[1], &params_[2], &params_[3]);
  }

  // problem->SetParameterLowerBound(&params_[0], 0, 0.0);   // A1 positive
  problem->SetParameterLowerBound(&params_[1], 0, -0.9*GetLambdaPhysicHours()); // positive
  // problem->SetParameterLowerBound(&params_[2], 0, 0.0);   // A2 positive
  problem->SetParameterLowerBound(&params_[3], 0, -0.9*GetLambdaPhysicHours());   // l2 positive


  //problem->SetParameterUpperBound(&params_[1], 0, 0.0);//GetLambdaPhysicHours()*2.0); // positive
  // problem->SetParameterLowerBound(&params_[3], 0, 0.1*GetLambdaPhysicHours()); // positive
  // problem->SetParameterUpperBound(&params_[3], 0, GetLambdaPhysicHours()); // positive
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitModelBase * syd::FitModel_f4::Clone() const
{
  auto * model = new syd::FitModel_f4;
  model->CopyFrom(this);
  return model;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template <typename T>
bool
syd::FitModel_f4::ResidualType::operator()(const T* const A1,
                                           const T* const l1,
                                           const T* const A2,
                                           const T* const l2,
                                           T* residual) const
{
  residual[0] = (T(y_) - (
                          A1[0]*exp(-(l1[0]+lambda) * T(x_)) +
                          A2[0]*exp(-(l2[0]+lambda) * T(x_))
                          ));
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f4::GetValue(const double & t) const
{
  const double A1 = params_[0];
  const double l1 = params_[1];
  const double A2 = params_[2];
  const double l2 = params_[3];
  const double l = lambda_phys_hours_;
  return A1 * exp(-(l+l1)*t) + A2 * exp(-(l+l2)*t);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f4::GetA(const int i) const
{
  if (i==0) return params_[0];
  else return params_[2];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f4::GetLambda(const int i) const
{
  if (i==0) return params_[1];
  else return params_[3];
}
// --------------------------------------------------------------------
