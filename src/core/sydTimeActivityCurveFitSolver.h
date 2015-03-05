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

#ifndef SYDTIMEACTIVITYCURVEFITSOLVER_H
#define SYDTIMEACTIVITYCURVEFITSOLVER_H

// syd
#include "sydTimeActivityCurve.h"

// Ceres
#include <ceres/ceres.h>

// syd
#include "sydTimeActivityCurve.h"

// --------------------------------------------------------------------
namespace syd {

  double TwoExponential(const double A, const double K1, const double K2, const double x);

  class TimeActivityCurveFitSolver
  {
  public:

    TimeActivityCurveFitSolver();
    ~TimeActivityCurveFitSolver();

    struct MonoExponentialResidual;
    struct MonoExponentialResidualWeighted;
    struct TwoExponentialResidual;

    void SetInput(TimeActivityCurve * tac);
    void InitIncrementalRun();
    void Run();
    void IncrementalRun();
    void Run_f4a();

    double GetFitLambda() const { return lambda_; }
    double GetFitError() const { return fit_error_; }
    double GetFitNbPoints() const { return nb_used_points_; }
    int GetFitNbOfIterations() const;
    std::string GetFitComment() const { return comment_; }
    double GetFitA() const { return A_; }
    void SetUseWeightedFit(bool b) { useWeightedFit_ = b; }
    ceres::Solver::Summary & GetSummary() { return ceres_summary_; }

    double GetFitK1() const { return K1_; }
    double GetFitK2() const { return K2_; }

  protected:
    double A_;
    double lambda_;
    double fit_error_;
    unsigned int nb_used_points_;
    unsigned int fit_nb_iterations_;
    TimeActivityCurve * tac_;
    std::string comment_;
    bool useWeightedFit_;

    // following variables for IncrementalRun
    unsigned int kNumObservations_;
    ceres::Solver::Options * ceres_options_;
    ceres::Solver::Summary ceres_summary_;
    std::vector<MonoExponentialResidual*> residuals_;
    typedef ceres::AutoDiffCostFunction<MonoExponentialResidual, 1, 1, 1> CostFctType1;
    double K1_;
    double K2_;

  };

}  // namespace syd
// --------------------------------------------------------------------

#endif
