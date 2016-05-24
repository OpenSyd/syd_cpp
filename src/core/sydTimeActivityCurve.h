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

#ifndef SYDTIMEACTIVITYCURVE_H
#define SYDTIMEACTIVITYCURVE_H

// syd
#include "sydDD.h"
#include "sydLog.h"

using namespace sydlog;

// std
#include <vector>
#include <memory>

// --------------------------------------------------------------------
namespace syd {

  class TimeActivityCurve {
  public:

    // Types
    typedef std::shared_ptr<TimeActivityCurve> pointer;
    typedef std::vector<pointer> vector;
    static pointer New() { return std::make_shared<syd::TimeActivityCurve>(); }

    // Constructor
    TimeActivityCurve();
    ~TimeActivityCurve();

    // Input
    void AddValue(double time, double value, double variance=0.0);
    void SetValue(unsigned int i, double v) { values[i] = v; }
    void SetTime(unsigned int i, double v) { times[i] = v; }

    // Get information
    unsigned int size() const { return times.size(); } // like vector
    unsigned int GetSize() const { return size(); }
    double GetValue(unsigned int i) const { return values[i]; }
    double GetTime(unsigned int i) const { return times[i]; }
    double GetVariance(unsigned int i) const { return variances[i]; }
    std::vector<double> GetTimes() const { return times; }
    std::vector<double> GetValues() const { return values; }
    std::vector<double> GetVariances() const { return variances; }

    // Functions
    void CopyFrom(syd::TimeActivityCurve & tac);
    void SortByTime();
    unsigned int FindIndexOfMaxValueFromTheEnd(unsigned int min_nb_of_values=3) const;
    unsigned int FindIndexOfMaxValue() const;
    double ComputeIntegralByTrapezoidRule(int start, int end) const;
    double ComputeIntegralBeforeFirstTimepoint() const;
    void Clear() { clear(); }
    void clear(); // like vector

    // Print
    friend std::ostream& operator<<(std::ostream& os, const TimeActivityCurve & p);
    friend std::ostream& operator<<(std::ostream& os, const TimeActivityCurve::pointer p);

  protected:
    std::vector<double> times;
    std::vector<double> values;
    std::vector<double> variances;

  };
}  // namespace syd
// --------------------------------------------------------------------

#endif
