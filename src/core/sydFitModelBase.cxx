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
#include "sydFitModelBase.h"

// --------------------------------------------------------------------
syd::FitModelBase::FitModelBase()
{
  name_ = "no_name";
  robust_scaling_ = 0.1;
  start_from_max_flag = false;
  id_ = 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModelBase::SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac)
{
  current_tac = &tac;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModelBase::CopyFrom(const syd::FitModelBase * model)
{
  name_ = model->GetName();
  lambda_phys_hours_ = model->GetLambdaPhysicHours();
  params_  = model->GetParameters();
  start_from_max_flag = model->start_from_max_flag;
  ceres_summary_ = model->ceres_summary_;
  current_tac = model->current_tac;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeActivityCurve * syd::FitModelBase::GetTAC(double first_time, double last_time, int n) const
{
  syd::TimeActivityCurve * tac = new syd::TimeActivityCurve;
  double step = (last_time-first_time)/(double)n;
  double time = first_time;
  for(auto i=0; i<n; i++) {
    tac->AddValue(time, GetValue(time));
    time += step;
  }
  return tac;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
namespace syd {
  std::ostream& operator<<(std::ostream& os, const FitModelBase & p)
  {
    //FIXME
    return os;
  }
} // end namespace
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModelBase::Integrate(double a, double b) const
{
  double x;
  for(auto k=0; k<GetNumberOfExpo(); k++) {
    double A = GetA(k);
    double l = GetLambda(k) + GetLambdaPhysicHours();
    x += A/l * (exp(-l*a) - exp(-l*b)) ;
  }
  return x;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModelBase::Integrate() const
{
  double x = 0.0;
  for(auto k=0; k<GetNumberOfExpo(); k++) {
    double A = GetA(k);
    double l = GetLambda(k) + GetLambdaPhysicHours();
    x += A/l;
  }
  return x;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModelBase::ComputeAUC(const syd::TimeActivityCurve & tac, bool use_current_tac) const
{
  // Simple integration if full model
  if (!use_current_tac) return Integrate();

  // If not, we consider the current_tac as the restricted one
  if (!current_tac) {
    LOG(FATAL) << "Could not compute ComputeAUC with restricted tac, 'current_tac' must be set";
  }
  double AUC = 0.0;

  // Integrate from 0 to first time of the restricted_tac
  double starting_part_model = Integrate(0.0, current_tac->GetTime(0));

  // Integrate from 0 to infinity
  double total = Integrate();

  // Trapeze intregration of the first curve part
  int index = 0;
  double t = current_tac->GetTime(0);
  while (tac.GetTime(index) < t) ++index;
  double starting_part = tac.Integrate_Trapeze(0, index);

  AUC = total - starting_part_model + starting_part;

  return AUC;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModelBase::ComputeR2(const syd::TimeActivityCurve & tac, bool use_current_tac) const
{
  const syd::TimeActivityCurve * current = &tac;
  if (use_current_tac) {
    if (!current_tac) {
      LOG(FATAL) << "Could not compute ComputeAUC with restricted tac, 'current_tac' must be set";
    }
    current = current_tac;
  }

  double mean = 0.0;
  for(auto i=0; i<current->size(); i++) mean += current->GetValue(i);
  mean = mean / (double)current->size();

  double SS_res = ComputeSS(*current);

  double SS_tot = 0.0;
  for(auto i=0; i<current->size(); i++) {
    SS_tot += pow(current->GetValue(i)-mean, 2);
  }

  double R2 = 1.0 - (SS_res/SS_tot);
  return R2;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModelBase::ComputeSS(const syd::TimeActivityCurve & tac) const
{
  double SS = 0.0;
  for(auto i=0; i<tac.size(); i++) {
    SS += pow(tac.GetValue(i)-GetValue(tac.GetTime(i)),2);
  }
  return SS;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::FitModelBase::IsAICcValid(int N) const
{
  double K = GetK();
  return (K+1+2) <= N;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModelBase::ComputeAICc(const syd::TimeActivityCurve & tac) const
{
  // See Glatting 2007 equ (4)
  // See Kletting 2013 eq (5) / (6) and erratum
  // See Burnham2011 and others
  double N = tac.size();
  double K = GetK();
  double SS = ComputeSS(tac);

  double AIC = N * log(SS/N) + 2.0*(K+1) ;  // std::log is ln (natural logarithm), least square fit with normally dist errors
  //double AICc = AIC + (2*(K+1.0)*(K+2.0))/(N-K-2.0); // relative -> but issue if N-K-2 too low.
  double AICc = AIC + (2*K*(K+1))/(N-K-1); // absolute

  if (K+2 > N)  AICc = 666; // not applicable
  //  if (K+3 > N)  AICc = 666; // not applicable

  // std::cout << std::endl << name_ << " N=" << N << " K=" << K << " SS= " << SS
  //           << " -> " << AIC << " " << AICc << std::endl;

  return AICc;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::FitModelBase::IsAcceptable() const
{
  bool is_ok = true;
  for(auto k=0; k<GetNumberOfExpo(); k++) {
    double A = GetA(k);
    double l = GetLambda(k) + GetLambdaPhysicHours();
    if (A<=0.0) is_ok = false; // Warning, to change for some model (f4a)
    if (l<0.5*GetLambdaPhysicHours()) is_ok = false; // too slow decay
  }
  return is_ok;
}
// --------------------------------------------------------------------
