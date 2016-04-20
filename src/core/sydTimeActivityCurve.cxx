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
#include "sydTimeActivityCurve.h"

// --------------------------------------------------------------------
syd::TimeActivityCurve::TimeActivityCurve()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeActivityCurve::~TimeActivityCurve()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurve::AddValue(double time, double value, double variance)
{
  times.push_back(time);
  values.push_back(value);
  variances.push_back(variance);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurve::SortByTime()
{
  LOG(FATAL) << "TODO TimeActivityCurve::SortByTime";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
unsigned int syd::TimeActivityCurve::FindMaxIndex()
{
  double max=0.0;
  unsigned int max_index=0;
  for (auto i = 0; i < size(); ++i) {
    if (GetValue(i) >= max) {
      max = GetValue(i);
      max_index = i;
    }
  }
  return max_index;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
namespace syd {
  std::ostream& operator<<(std::ostream& os, const TimeActivityCurve & p)
  {
    os << "(" << p.size() << ") ";
    for(auto i=0; i<p.size(); i++)
      os << p.GetTime(i) << " " << p.GetValue(i) << " ; ";
    return os;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::TimeActivityCurve::Integrate_Trapeze(int start, int end) const
{
  double r = 0.0;
  if (end >= size()-1) {
    LOG(FATAL) << "Could not Integrate_Trapeze with end index = " << end;
  }
  for(int i=start; i<end; ++i) {
    // First solution with paralelogram
    double a = GetValue(i);
    double b = GetValue(i+1);
    double d = GetTime(i+1)-GetTime(i);
    r = r + d*(a+b)/2.0;
    /*
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
    */

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
void syd::TimeActivityCurve::clear()
{
  times.clear(); values.clear(); variances.clear();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurve::CopyFrom(syd::TimeActivityCurve & tac)
{
  clear();
  for(auto i=0; i<tac.size(); i++) {
    times.push_back(tac.GetTime(i));
    values.push_back(tac.GetValue(i));
    variances.push_back(tac.GetVariance(i));
  }
}
// --------------------------------------------------------------------
