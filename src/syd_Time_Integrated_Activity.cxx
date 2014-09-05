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

#include "syd_Time_Integrated_Activity.h"
#include "sydFit.h"

// --------------------------------------------------------------------
syd::Time_Integrated_Activity::
Time_Integrated_Activity():With_Verbose_Flag()
{
  Set_Nb_Of_Points_For_Fit(2);
  Init();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Time_Integrated_Activity::
Set_Data(const std::vector<double> & _times,
         const std::vector<double> & _activities,
         const std::vector<double> & _std)
{
  assert(_times.size() == _activities.size());
  assert(_times.size() == _stddev.size());
  times = &_times;
  activities = &_activities;
  std = &_std;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Time_Integrated_Activity::
Set_Data(const std::vector<double> & _times,
         const std::vector<double> & _activities)
{
  std::vector<double> * a = new std::vector<double>(_times.size());
  std::fill(a->begin(), a->end(), 1.0); // no std deviation
  Set_Data(_times, _activities, *a);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Time_Integrated_Activity::Init()
{
  m_Parameters.resize(2);
  f.Init();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Time_Integrated_Activity::Integrate()
{
  // DDS(*times);

  // FIXME : switch according to integration options


  // Local variables ref
  const std::vector<double> & X = *times;
  const std::vector<double> & Y = *activities;
  const std::vector<double> & S = *std;

  // First part of the curve, before first point
  // Use the two first points to linear interpolate to time zero
  double a = Integrate_From_Zero_To_First_Point(X, Y);

  // Second part of the curve, all expect the last ones
  // Parallelogram method from 0 to n (all points excepted the last n)
  int n = m_Nb_Of_Points_For_Fit;
  double b = Integrate_Parallelogram_method(X, Y, 0, n);

  // Third part of the curve, fitting process
  // Consider the last n points
  std::vector<double> Xend;
  std::vector<double> Yend;
  std::vector<double> Send;
  for(unsigned int i=X.size()-n; i<X.size(); i++) {
    Xend.push_back(X[i]);
    Yend.push_back(Y[i]);
    Send.push_back(S[i]);
  }
  // Fitting process
  // syd::Fit_Time_Activity f;
  f.Set_Data(Xend, Yend, Send);
  f.Fit_With_Mono_Expo();
  // Get Fit result
  double rms = f.Get_RMS();
  double A = f.Get_Parameter(0);
  double lambda = f.Get_Parameter(1);

  m_Parameters[0] = A;
  m_Parameters[1] = lambda;

  // DD(rms);
  // DD(A);
  // DD(lambda);
  // Integration
  double total = Integrate_MonoExponential(A, lambda);
  double firstpart = Integrate_MonoExponential(A, lambda, 0, X[X.size()-n]);
  double c = total-firstpart;

  // End
  m_IntegratedActivity = (a+b+c);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::Time_Integrated_Activity::Get_Integrated_Activity()
{
  return m_IntegratedActivity;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::Time_Integrated_Activity::
Integrate_From_Zero_To_First_Point(const std::vector<double> & X,
                                   const std::vector<double> & Y)
{
  double area = Y[0]*X[0]/2.0;
  return area;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::Time_Integrated_Activity::
Integrate_Parallelogram_method(const std::vector<double> & X,
                               const std::vector<double> & Y,
                               int start,
                               int n)
{
  double total = 0.0;
  for(unsigned int i=start; i<X.size()-n; i++) {
    double minY = std::min(Y[i], Y[i+1]);
    double maxY = std::max(Y[i], Y[i+1]);
    double d = X[i+1]-X[i];
    total = total + d*(maxY+minY)/2.0;
  }
  return total;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::Time_Integrated_Activity::
Integrate_MonoExponential(double A, double lambda, double start, double end)
{
  // integrate between a and b
  double r = -A/lambda * (exp(-lambda*end) - exp(-lambda*start));
  return r;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
double syd::Time_Integrated_Activity::
Integrate_MonoExponential(double A, double lambda)
{
  // integrate between 0 and inf
  return A/lambda;
}
// --------------------------------------------------------------------
