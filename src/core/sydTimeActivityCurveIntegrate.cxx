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
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurveIntegrate::Run()
{
  double r = 0.0;

  // From zero to first point, find value at time=0
  double slope =
    (tac_->GetValue(0)-tac_->GetValue(1)) /
    (tac_->GetTime(0)-tac_->GetTime(1));
  double intercept = tac_->GetValue(0) - slope * tac_->GetTime(0);
  r = tac_->GetTime(0)*(tac_->GetValue(0) + intercept)/2.0;
  // DD(r);

  // Fit
  solver.SetInput(tac_);
  solver.Run();
  int last = tac_->size()-1;// - solver.GetFitNbPoints();

  // Parallelogram method
  for(auto i=0; i<last; i++) {
    double a = tac_->GetValue(i);
    double b = tac_->GetValue(i+1);
    double d = tac_->GetTime(i+1)-tac_->GetTime(i);
    r = r + d*(a+b)/2.0;
  }
  // DD(r);

  // Last part, from last point to infinity with the fit parameters
  double A = solver.GetFitA();
  double lambda = solver.GetLambda();
  double start = 0.0;
  double end = tac_->GetTime(last);
  //  double maxpoint = 250; // 300 hours ?
  double ZeroToInf = A/lambda;
  double ZeroToLastPoint = -A/lambda * (exp(-lambda*end) - exp(-lambda*start));
  //double ZeroToMaxPoint = -A/lambda * (exp(-lambda*maxpoint) - exp(-lambda*start));
  r = r + (ZeroToInf-ZeroToLastPoint);
  //  r = r + (ZeroToInf-ZeroToLastPoint) - (ZeroToInf-ZeroToMaxPoint);
  //r = r + ZeroToMaxPoint - ZeroToLastPoint;
  //DD(r);
  //DD(ZeroToMaxPoint);
  //DD(log(2.0)/lambda);
  integrated_value_ = r;
}
// --------------------------------------------------------------------
