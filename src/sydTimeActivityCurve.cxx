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
#include "clitkDD.h"

// --------------------------------------------------------------------
double syd::Tac_MonoExpo(double A, double lambda, double t) {
  return A*exp(-lambda*t);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::Tac_Integrate(const std::vector<double> & X,
                          const std::vector<double> & Y,
                          int n,
                          double & rms,
                          double & A,
                          double & lambda)
{
  // Use the two first points to linear interpolate to time zero
  double a = Tac_Integrate_From_Zero_To_First_Point(X, Y);

  // Parallelogram for all points excepted the last n-1 ones
  double b = Tac_Integrate_Parallelogram_method(X, Y, n);

  // Fit last n points
  double c;
  std::vector<double> Xend;
  std::vector<double> Yend;
  for(unsigned int i=X.size()-n; i<X.size(); i++) {
    Xend.push_back(X[i]);
    Yend.push_back(Y[i]);
  }
  Tac_FitMonoExpo(Xend, Yend, A, lambda);

  rms = Tac_RMS(Xend, Yend, A, lambda);

  double total = Tac_Integrate(A, lambda);
  double firstpart = Tac_Integrate(A, lambda, 0, X[X.size()-n]);
  c = total-firstpart;

  /*
   // Use last n points to fit
  double c = Tac_Integrate_From_Last_To_Infinity(X, Y, n, rms, A, lambda);
  DD(c);

  // Use fit model to integrate last part of the curve rather than parallelogram
  int l = X.size()-1; // last
  DD(X[l-n+1]);
  DD(X[l]);
  double b2 = Tac_Integrate(A, lambda, X[l-n+1], X[l]);
  DD(b2);
  */

  // Final result
  //  return (a+b+c+b2);
  return (a+b+c);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::Tac_Integrate_From_Zero_To_First_Point(const std::vector<double> & X,
                                                   const std::vector<double> & Y)
{
  /*
  double x1=X[0];
  double x2=X[1];
  double y1=Y[0];
  double y2=Y[1];
  double a = (y1-y2)/(x1-x2);
  double b = y1-a*x1;
  double area = x1*b + (y1-b)*x1/2.0;
  return area;
  */
  double area = Y[0]*X[0]/2.0;
  return area;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::Tac_Integrate_Parallelogram_method(const std::vector<double> & X,
                                               const std::vector<double> & Y,
                                               int n)
{
  double total = 0.0;
  assert(n>0);
  for(unsigned int i=0; i<X.size()-n; i++) {
    double minY = std::min(Y[i], Y[i+1]);
    double maxY = std::max(Y[i], Y[i+1]);
    double d = X[i+1]-X[i];
    /*std::cout << X[i] << " " << X[i+1]
              << " " << Y[i] << " " << Y[i+1]
              << " " << d << " " << (maxY+minY)/2.0
              << " " << d*(maxY+minY)/2.0
              << std::endl;*/
    total = total + d*(maxY+minY)/2.0;
  }
  return total;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::Tac_Integrate_From_Last_To_Infinity(const std::vector<double> & X,
                                                const std::vector<double> & Y,
                                                int n,
                                                double & rms,
                                                double & A,
                                                double & lambda)
{
  std::vector<double> Xend;
  std::vector<double> Yend;
  for(unsigned int i=X.size()-n; i<X.size(); i++) {
    Xend.push_back(X[i]);
    Yend.push_back(Y[i]);
  }
  DD(n);
  double a,b;
  Tac_FitMonoExpo(Xend, Yend, A, lambda);
  DD(A);
  DD(lambda);
  double v = Tac_Integrate(A, lambda); // A/lambda; // integrate between 0 and inf
  double w = Tac_Integrate(A, lambda, 0, Xend.back()); // integrate between a and b
  DD(v);
  DD(w);
  v = v - w;
  DD(v);

  // compute rms
  rms = Tac_RMS(Xend,Yend,A,lambda);
  return v;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::Tac_RMS(const std::vector<double> & X,
                    const std::vector<double> & Y,
                    double A, double lambda)
{
  double rms = 0.0;
  for(auto i=0; i<X.size(); i++) {
    rms += pow((Y[i] - Tac_MonoExpo(A, lambda, X[i])),2);
  }
  rms = sqrt(rms/(double)X.size());
  return rms;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// y = A e(Bx)
// solve : ln y = ln A + B ln x
// So at the end A = exp(a) and B = b
void syd::Tac_FitMonoExpo(const std::vector<double> & X,
                          const std::vector<double> & Y,
                          double & A, double & lambda)
{
  // Number of points
  int n = X.size();
  // Partial sums
  double sum_x2y = 0.0;
  double sum_ylny = 0.0;
  double sum_xy = 0.0;
  double sum_y = 0.0;
  double sum_xylny = 0.0;
  for(unsigned int i=0; i<n; i++) {
    sum_x2y += X[i]*X[i]*Y[i];
    sum_ylny += Y[i]*log(Y[i]);
    sum_xy += X[i]*Y[i];
    sum_y += Y[i];
    sum_xylny += X[i]*Y[i] * log(Y[i]);
  }
  // a and b
  double a = (sum_x2y*sum_ylny) - (sum_xy*sum_xylny);
  double denom = sum_y*sum_x2y - (sum_xy*sum_xy);
  a = a/denom;
  double b = (sum_y*sum_xylny - sum_xy*sum_ylny)/denom;

  // final
  A = exp(a);
  lambda = -b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
double syd::Tac_Integrate(double A, double lambda, double start, double end)
{
  // integrate between a and b
  double r = -A/lambda * (exp(-lambda*end) - exp(-lambda*start));
  return r;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
double syd::Tac_Integrate(double A, double lambda)
{
  // integrate between 0 and inf
  return A/lambda;
}
//--------------------------------------------------------------------
