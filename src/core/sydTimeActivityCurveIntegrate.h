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

#ifndef SYDTIMEACTIVITYCURVEINTEGRATE_H
#define SYDTIMEACTIVITYCURVEINTEGRATE_H

// syd
#include "sydTimeActivityCurve.h"
#include "sydTimeActivityCurveFitSolver.h"

// Ceres
#include <ceres/ceres.h>
#include <glog/logging.h>

// --------------------------------------------------------------------
namespace syd {

  class TimeActivityCurveIntegrate
  {
  public:

    TimeActivityCurveIntegrate();
    ~TimeActivityCurveIntegrate();

    void SetInput(TimeActivityCurve * tac);
    void Run();
    TimeActivityCurveFitSolver & GetSolver() { return solver; }
    double GetIntegratedValue() { return integrated_value_; }

  protected:
    TimeActivityCurve * tac_;
    TimeActivityCurveFitSolver solver;
    double integrated_value_;
  };

}  // namespace syd
// --------------------------------------------------------------------

#endif
