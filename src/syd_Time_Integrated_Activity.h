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

#ifndef SYD_TIME_INTEGRATED_ACTIVITY_H
#define SYD_TIME_INTEGRATED_ACTIVITY_H

// syd
#include "sydQuery.h"
#include "syd_With_Verbose_Flag.h"

namespace syd {

  // --------------------------------------------------------------------
  class Time_Integrated_Activity : public syd::With_Verbose_Flag {

  public:
    Time_Integrated_Activity();

    void Set_Data(const std::vector<double> & times,
                  const std::vector<double> & activities,
                  const std::vector<double> & std);

    void Set_Nb_Of_Points_For_Fit(int n) { m_Nb_Of_Points_For_Fit = n; }

    static double Integrate_From_Zero_To_First_Point(const std::vector<double> & X, const std::vector<double> & Y);
    static double Integrate_Parallelogram_method(const std::vector<double> & X, const std::vector<double> & Y,
                                                 int start, int n);
    static double Integrate_MonoExponential(double A, double lambda, double start, double end);
    static double Integrate_MonoExponential(double A, double lambda);

    void Integrate();
    double Get_Integrated_Activity();

  protected:
    int m_Nb_Of_Points_For_Fit;
    const std::vector<double> * times;
    const std::vector<double> * activities;
    const std::vector<double> * std;
    double m_IntegratedActivity;

  };
  // --------------------------------------------------------------------

} // end namespace

#endif
