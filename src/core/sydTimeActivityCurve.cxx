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
  unsigned int max_index;
  for (auto i = 0; i < size(); ++i) {
    if (GetValue(i) > max) {
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
