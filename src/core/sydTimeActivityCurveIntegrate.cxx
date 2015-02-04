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
#include "sydTimeActivityCurveIntegrate.h"
#include "sydTimeActivityCurve.h"

// --------------------------------------------------------------------
syd::TimeActivityCurveIntegrate::TimeActivityCurveIntegrate()
{
  integrated_value_ = 0.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeActivityCurveIntegrate::~TimeActivityCurveIntegrate()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurveIntegrate::SetInput(TimeActivityCurve * tac)
{
  tac_ = tac;
  solver.SetInput(tac);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::TimeActivityCurveIntegrate::ComputeFirstPointIntegration()
{
  // From zero to first point, find value at time=0
  /*
  double slope =
    (tac_->GetValue(0)-tac_->GetValue(1)) /
    (tac_->GetTime(0)-tac_->GetTime(1));
  double intercept = tac_->GetValue(0) - slope * tac_->GetTime(0);
  double r = tac_->GetTime(0)*(tac_->GetValue(0) + intercept)/2.0;
  */
  double t1 = tac_->GetTime(0);
  double t2 = tac_->GetTime(1);
  double A1 = tac_->GetValue(0);
  double A2 = tac_->GetValue(1);
  double r = A1*t1 - t1*t1*(A1-A2)/(t1-t2)/2.0;
  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::TimeActivityCurveIntegrate::ComputeParallelogramIntegration()
{
 int last = tac_->size()-1;
 double r = 0.0;
 for(auto i=0; i<last; i++) {

   // First solution with paralelogram
   /*
     double a = tac_->GetValue(i);
     double b = tac_->GetValue(i+1);
     double d = tac_->GetTime(i+1)-tac_->GetTime(i);
     r = r + d*(a+b)/2.0;
   */

   // Alternative : fit monoexpo curve at each point
   double t1 = tac_->GetTime(i);
   double t2 = tac_->GetTime(i+1);
   double A1 = tac_->GetValue(i);
   double A2 = tac_->GetValue(i+1);
   double lambda = 1.0/((t2-t1)/log(A1/A2));
   double A = A1/exp(-lambda*t1);
   //   double B = A2/exp(-lambda*t2);
   double integral = syd::IntegrateMonoExpo(A, lambda, t1, t2);
   r += integral;

   /* debug
   std::cout << t1 << " " << A1 << "      " << t2 << " " << A2 << std::endl;
   std::cout << "paral = " << d*(a+b)/2.0 << " and int= " << integral << std::endl;
   std::cout << lambda << " " << A << " " << B << std::endl;
   */
 }
 return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::TimeActivityCurveIntegrate::ComputeFinalCurveIntegration()
{
  int last = tac_->size()-1;// - solver.GetFitNbPoints();
  double A = solver.GetFitA();
  double lambda = solver.GetFitLambda();
  double start = 0.0;
  double end = tac_->GetTime(last);
  //  double maxpoint = 250; // 300 hours ?
  double ZeroToInf = A/lambda;
  double ZeroToLastPoint = -A/lambda * (exp(-lambda*end) - exp(-lambda*start));
  //double ZeroToMaxPoint = -A/lambda * (exp(-lambda*maxpoint) - exp(-lambda*start));
  double r = ZeroToInf-ZeroToLastPoint;
  //  r = r + (ZeroToInf-ZeroToLastPoint) - (ZeroToInf-ZeroToMaxPoint);
  //r = r + ZeroToMaxPoint - ZeroToLastPoint;
  //DD(r);
  //DD(ZeroToMaxPoint);
  //DD(log(2.0)/lambda);
 return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurveIntegrate::Run()
{
  // From zero to first point, find value at time=0
  double r = ComputeFirstPointIntegration();

  // Parallelogram method
  r += ComputeParallelogramIntegration();

  // Last part, from last point to infinity with the fit parameters
  solver.SetInput(tac_);
  solver.Run();
  r += ComputeFinalCurveIntegration();
  integrated_value_ = r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurveIntegrate::IncrementalRun()
{
  // From zero to first point, find value at time=0
  temp_FirstPart_ = ComputeFirstPointIntegration();
  double r = temp_FirstPart_;

  // Parallelogram method
  temp_MiddlePart_ = ComputeParallelogramIntegration();
  r += temp_MiddlePart_;

  // Last part, from last point to infinity with the fit parameters
  solver.IncrementalRun();
  temp_FinalPart_ = ComputeFinalCurveIntegration();
  r += temp_FinalPart_;
  integrated_value_ = r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::TimeActivityCurveIntegrate::GetIntegratedValueWithConditions()
{
  double v = integrated_value_;
  if (!std::isfinite(v)) return 0.0;
  if (std::isnan(v)) return 0.0;
  if (v < 0) return 0.0;
  return v;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::IntegrateMonoExpo(double A, double lambda, double start, double end)
{
  double r = -A/lambda * (exp(-lambda*end) - exp(-lambda*start));
  return r;
}
// --------------------------------------------------------------------
