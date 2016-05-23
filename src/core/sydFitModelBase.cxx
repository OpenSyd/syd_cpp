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
void syd::FitModelBase::SetParameters(std::vector<double> & p)
{
  params_.clear();
  params_ = p;
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
  double x=0.0;
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
double syd::FitModelBase::ComputeAUC_OLD(const syd::TimeActivityCurve & tac, bool use_current_tac) const
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
double syd::FitModelBase::ComputeAUC(const syd::TimeActivityCurve::pointer tac, int index) const
{
  double AUC = 0.0;

  // Integrate from 0 to first time of the restricted tac
  double starting_part_model = Integrate(0.0, tac->GetTime(index)); //params are times

  // Integrate from 0 to infinity
  double total = Integrate();

  // Trapeze intregration of the first curve part
  double paralelogram_part = tac->Integrate_Trapeze(0, index); // params are index

  // Consider from times=0 to the first time, according to slope between 2 first timepoints
  double r = tac->GetIntegralBeforeFirstTimepoint();

  // Final AUC is total integration, minus start model integration, plus trapez part.
  AUC = total - starting_part_model + paralelogram_part + r;

  DDS(GetParameters());
  std::cout << "Compute auc tmax=" << tac->GetTime(index)
            << " total= " << total
            << " start_part=" << starting_part_model
            << " trapez= " << paralelogram_part
            << " r=" << r
            << "   --> " << AUC << std::endl;

  return AUC;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModelBase::ComputeR2(const syd::TimeActivityCurve::pointer tac) const
{
  double mean = 0.0;
  for(auto i=0; i<tac->size(); i++) mean += tac->GetValue(i);
  mean = mean / (double)tac->size();

  double SS_res = ComputeRSS(tac);

  double SS_tot = 0.0;
  for(auto i=0; i<tac->size(); i++) {
    SS_tot += pow(tac->GetValue(i)-mean, 2);
  }

  double R2 = 1.0 - (SS_res/SS_tot);
  return R2;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModelBase::ComputeRSS(const syd::TimeActivityCurve::pointer tac) const
{
  double SS = 0.0;
  for(auto i=0; i<tac->size(); i++) {
    SS += pow(tac->GetValue(i)-GetValue(tac->GetTime(i)),2);
  }
  return SS;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::FitModelBase::IsAICcValid(int N) const
{
  double K = GetK();
  //  return (K+1+2) <= N; // relative weighting
  return (K+2) <= N; // for absolute weighting
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModelBase::ComputeAIC(const syd::TimeActivityCurve::pointer tac) const
{
  // See Glatting 2007 equ (4)
  // See Kletting 2013 eq (5) / (6) and erratum
  // See Burnham2011 and others
  double N = tac->size();
  double K = GetK();
  double RSS = ComputeRSS(tac); // RSS residual sum of squares

  // std::log is ln (natural logarithm), least square fit with normally dist errors
  // See Glatting2007 eq(4)
  // See Burnham2011
  double L = N*std::log(RSS/N);// likelihood FIXME / N ???
  double AIC = L + 2*K; //N * std::log(SS/N) + 2.0*(K+1) ;

  // AIC = k + n [Ln( 2(pi) RSS/(n-k) ) +1],
  // AIC = K + N*(std::log(2*M_PI) * RSS/(N-K) + 1.0);

  //  double BIC = L + (K+1)*log(N);
  //return BIC;
  return AIC;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModelBase::ComputeAICc(const syd::TimeActivityCurve::pointer tac) const
{
  // See Glatting 2007 equ (4)
  // See Kletting 2013 eq (5) / (6) and erratum
  // See Burnham2011 and others
  double N = tac->size();
  double K = GetK();

  // std::log is ln (natural logarithm), least square fit with normally dist errors
  // See Glatting2007 eq(4)
  double AIC = ComputeAIC(tac);
  // See Kletting2014
  //  double AICc = AIC + (2.0*K*(K+1.0))/(N-K-1.0); // absolute
  double d = N-K-1.0;
  if (N-K-1.0 < 1.0) d = 1;
  double AICc = AIC +  (2.0*K*(K+1.0))/d;//(N-K-1.0);

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
    // if (A<=0.0) is_ok = false; // Warning, to change for some model (f4a)
    // if (l<0.2*GetLambdaPhysicHours()) is_ok = false; // too slow decay
  }
  return is_ok;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::FitModelBase::GetEffHalfLife() const
{
  double h = GetLambda(0) + GetLambdaPhysicHours();
  return log(2.0)/h;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// Fit y = c*exp(d*x)
// linearize : log(y) = log(c) + dx
void syd::FitModelBase::LogLinearFit(Eigen::Vector2d & x,
                                     const syd::TimeActivityCurve::pointer tac,
                                     int start, int end)
{
  if (end == -1) end = tac->size();
  int n = end-start;
  Eigen::MatrixXd A(n,2);
  Eigen::VectorXd b(n);
  for(auto i=start; i<end; i++) {
    A(i-start,0) = 1.0;
    A(i-start,1) = tac->GetTime(i);
    b(i-start) = log(tac->GetValue(i));
  }
  x = A.householderQr().solve(b);
  x(0) = exp(x(0));
}
// --------------------------------------------------------------------
