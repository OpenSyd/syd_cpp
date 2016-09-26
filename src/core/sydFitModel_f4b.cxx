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
#include "sydFitModel_f4b.h"

// --------------------------------------------------------------------
syd::FitModel_f4b::FitModel_f4b():FitModelBase()
{
  name_ = "f4b";
  params_.resize(3);
  id_ = 6;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModel_f4b::SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac)
{
  syd::FitModelBase::SetProblemResidual(problem, tac);

  // Initialisation
  params_[0] = tac.GetValue(0); // A1
  params_[1] = 0.0; // lambda_1
  params_[2] = 0.0; // lambda_2

  // need to be created each time
  residuals_.clear();
  for(auto i=0; i<tac.size(); i++) {
    auto r = new ResidualType(tac.GetTime(i), tac.GetValue(i), GetLambdaDecayConstantInHours());
    residuals_.push_back(r);
  }
  // FIXME --> could be templated by CostFctType and param_nb ?
  for(auto i=0; i<tac.size(); i++) {
    problem->AddResidualBlock(new CostFctType(residuals_[i]), NULL, &params_[0], &params_[1], &params_[2]);
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitModelBase * syd::FitModel_f4b::Clone() const
{
  auto * model = new syd::FitModel_f4b;
  model->CopyFrom(this);
  return model;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template <typename T>
bool
syd::FitModel_f4b::ResidualType::operator()(const T* const A1,
                                           const T* const lambda_1,
                                           const T* const lambda_2,
                                           T* residual) const
{
  residual[0] = (T(y_) - (
                          A1[0]*exp(-(lambda_1[0]+lambda) * T(x_)) +
                          (100.0-A1[0])*exp(-(lambda_2[0]+lambda) * T(x_))
                          ));
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f4b::GetValue(const double & t) const
{
  const double A1 = params_[0];
  const double lambda_1 = params_[1];
  const double lambda_2 = params_[2];
  const double l = lambda_in_hours_;
  return
    A1 * exp(-(l+lambda_1)*t) +
    (100.0-A1)*exp(-(l+lambda_2)*t);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f4b::GetA(const int i) const
{
  if (i==0) return params_[0];
  else return 100.0-params_[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f4b::GetLambda(const int i) const
{
  if (i==0) return params_[1];
  else return params_[2];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModel_f4b::Scale(double s)
{
  params_[0] *= s;
}
// --------------------------------------------------------------------
