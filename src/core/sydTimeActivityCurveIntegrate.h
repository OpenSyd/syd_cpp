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
    void IncrementalRun();

    TimeActivityCurveFitSolver & GetFitSolver() { return solver; }
    ceres::Solver::Summary & GetSolverSummary() { return solver.GetSummary(); }
    double GetIntegratedValue() { return integrated_value_; }
    double GetTempFirstPartIntegration() { return temp_FirstPart_; }
    double GetTempMiddlePartIntegration() { return temp_MiddlePart_; }
    double GetTempFinalPartIntegration() { return temp_FinalPart_; }
    double GetIntegratedValueWithConditions();

  protected:
    double ComputeFirstPointIntegration();
    double ComputeParallelogramIntegration();
    double ComputeFinalCurveIntegration();

    TimeActivityCurve * tac_;
    TimeActivityCurveFitSolver solver;
    double integrated_value_;
    double temp_FirstPart_;
    double temp_MiddlePart_;
    double temp_FinalPart_;
  };

  double IntegrateMonoExpo(double A, double lambda, double start, double end);

}  // namespace syd
// --------------------------------------------------------------------

#endif
