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
#include "sydCommon.h"
#include <random>

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
  auto p = syd::sort_permutation(times,
                                 [](double const& a, double const& b){ return (a<b); });
  times = syd::apply_permutation(times, p);
  values = syd::apply_permutation(values, p);
  variances = syd::apply_permutation(variances, p);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
unsigned int syd::TimeActivityCurve::FindIndexOfMaxValueFromTheEnd(unsigned int min_nb_of_values) const
{
  double previous_value = std::numeric_limits<double>::lowest();
  int i; // not unsigned (required for the loop)
  for(i=size()-1; i>=0; i--) {
    if (GetValue(i) < previous_value) break;
    else previous_value = GetValue(i);
  }
  i++;
  i = std::min(i, (int)(size()-min_nb_of_values));
  return i;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
unsigned int syd::TimeActivityCurve::FindIndexOfMaxValue() const
{
  double max = std::numeric_limits<double>::lowest();
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

  std::ostream& operator<<(std::ostream& os, const TimeActivityCurve::pointer p)
  {
    return os << *p;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::TimeActivityCurve::ComputeIntegralBeforeFirstTimepoint() const
{
  double a = GetTime(0);
  double b = GetTime(1);
  double va = GetValue(0);
  double vb = GetValue(1);
  double m = (vb-va)/(b-a);
  double c = va - m*a;
  double r = a*(va + c)/2.0;
  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::TimeActivityCurve::ComputeIntegralByTrapezoidRule(int start, int end) const
{
  double r = 0.0;
  double a = GetTime(start);
  double b = GetTime(end);
  r = (b-a)*((GetValue(start)+GetValue(end))/2.0);
  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurve::clear()
{
  times.clear();
  values.clear();
  variances.clear();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeActivityCurve::pointer syd::TimeActivityCurve::GetPoissonNoiseTAC(std::default_random_engine & generator)
{
  auto tac = New();
  for(auto i=0; i<size(); i++){
    auto v = GetValue(i);
    std::poisson_distribution<int> distribution(v);
    v = distribution(generator);
    tac->AddValue(times[i], v);
  }
  return tac;
}
// --------------------------------------------------------------------
