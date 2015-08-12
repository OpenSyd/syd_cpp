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
#include "sydFitModel_f4c.h"

// --------------------------------------------------------------------
syd::FitModel_f4c::FitModel_f4c():FitModelBase()
{
  name_ = "f4c";
  params_.resize(3);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModel_f4c::SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac)
{
  syd::FitModelBase::SetProblemResidual(problem, tac);

  // Initialisation
  params_[0] = GetLambdaPhysicHours(); // l_r
  params_[1] = 0.0; // l_t
  params_[2] = 0.0; // l_h

  // need to be created each time
  residuals_.clear();
  for(auto i=0; i<tac.size(); i++) {
    auto r = new ResidualType(tac.GetTime(i), tac.GetValue(i), GetLambdaPhysicHours());
    residuals_.push_back(r);
  }
  // FIXME --> could be templated by CostFctType and param_nb ?
  for(auto i=0; i<tac.size(); i++) {
    problem->AddResidualBlock(new CostFctType(residuals_[i]), NULL, &params_[0], &params_[1], &params_[2]);
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitModelBase * syd::FitModel_f4c::Clone() const
{
  auto * model = new syd::FitModel_f4c;
  model->CopyFrom(this);
  return model;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template <typename T>
bool
syd::FitModel_f4c::ResidualType::operator()(const T* const lr,
                                            const T* const lt,
                                            const T* const lh,
                                            T* residual) const
{
  residual[0] = (T(y_) - (
                          100.0 * (lt[0]/(lr[0]+lt[0]-lh[0])) *
                          (exp(-(lh[0]+lambda) * T(x_)) -
                           exp(-(lr[0]+lt[0]+lambda) * T(x_)))
                          ));
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f4c::GetValue(const double & t) const
{
  const double lr = params_[0];
  const double lt = params_[1];
  const double lh = params_[2];
  const double l = lambda_phys_hours_;
  return 100 * lt/(lr+lt-lh) *
    (exp(-(lh+l)*t) - exp(-(lr+lt+l)*t));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f4c::GetA(const int i) const
{
  const double lr = params_[0];
  const double lt = params_[1];
  const double lh = params_[2];
  return lt/(lr+lt-lh);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModel_f4c::GetLambda(const int i) const
{
  if (i==0) return params_[2];
  else return params_[1]+params_[0];
}
// --------------------------------------------------------------------
