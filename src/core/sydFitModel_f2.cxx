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
#include "sydFitModel_f2.h"

// --------------------------------------------------------------------
syd::FitModel_f2::FitModel_f2():FitModelBase()
{
  name_ = "f2";
  params_.resize(2);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModel_f2::SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac)
{
  // Initialisation
  params_[0] = tac.GetValue(0); // A1
  params_[1] = 0.0; // lambda_1

  // need to be created each time
  residuals_.clear();
  for(auto i=0; i<tac.size(); i++) {
    auto r = new ResidualType(tac.GetTime(i), tac.GetValue(i), GetLambdaPhysicHours());
    residuals_.push_back(r);
  }
  // FIXME --> could be templated by CostFctType and param_nb ?
  for(auto i=0; i<tac.size(); i++) {
    problem->AddResidualBlock(new CostFctType(residuals_[i]), NULL, &params_[0], &params_[1]);
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitModelBase * syd::FitModel_f2::Clone() const
{
  auto * model = new syd::FitModel_f2;
  model->CopyFrom(this);
  return model;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template <typename T>
bool
syd::FitModel_f2::ResidualType::operator()(const T* const A,
                                           const T* const lambda_1,
                                           T* residual) const
{
  residual[0] = (T(y_) - A[0]*exp(-(lambda_1[0]+lambda) * T(x_)));
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f2::GetValue(const double & time) const
{
  const double A1 = params_[0];
  const double lambda_1 = params_[1];
  const double l = lambda_phys_hours_;
  return A1 * exp(-(l+lambda_1)*time);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f2::GetA(const int i) const
{
  return params_[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f2::GetLambda(const int i) const
{
  return params_[1];
}
// --------------------------------------------------------------------
