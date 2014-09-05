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
#include "sydCommon.h"

namespace syd {

  static double Lambda_Indium = log(2.0)/(24.0 * 2.8047); // in hours

  //--------------------------------------------------------------------
  double Tac_MonoExpo(double A, double lambda, double t);

  double Tac_Integrate(const std::vector<double> & X,
                       const std::vector<double> & Y,
                       int n, double & rms,
                       double & A, double & lambda);

  double Tac_Integrate(double A, double lambda, double start, double end);
  double Tac_Integrate(double A, double lambda); // from 0 to infinity

  double Tac_Integrate_From_Zero_To_First_Point(const std::vector<double> & X,
                                                const std::vector<double> & Y);

  double Tac_Integrate_Parallelogram_method(const std::vector<double> & X,
                                            const std::vector<double> & Y,
                                            int n);

  double Tac_Integrate_From_Last_To_Infinity(const std::vector<double> & X,
                                             const std::vector<double> & Y,
                                             int n, double & rms,
                                             double & A, double & lambda);

  double Tac_RMS(const std::vector<double> & X,
                 const std::vector<double> & Y,
                 double A, double lambda);

  void Tac_FitMonoExpo(const std::vector<double> & X,
                       const std::vector<double> & Y,
                       double & A, double & lambda);

}

#endif
