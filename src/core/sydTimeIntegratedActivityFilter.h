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

#ifndef SYDTIMEINTEGRATEDACTIVITYFILTER_H
#define SYDTIMEINTEGRATEDACTIVITYFILTER_H

// syd
#include "sydFitModels.h"
#include "sydTimeActivityCurve.h"
#include "sydTimeIntegratedActivityFitOptions.h"

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to compute a time integrated activity.
  class TimeIntegratedActivityFilter {

  public:
    /// Constructor
    TimeIntegratedActivityFilter();

    /// Destructor (empty)
    ~TimeIntegratedActivityFilter();

    /// Input
    void SetTAC(syd::TimeActivityCurve::pointer tac) { initial_tac_ = tac; }
    void SetOptions(syd::TimeIntegratedActivityFitOptions & options) { options_ = options; }
    //void AddTimePointValue(double time, double value);

    /// Main function
    void Run();

    /// Output
    syd::TimeActivityCurve::pointer GetCurrentTAC() const { return initial_tac_; }
    syd::TimeActivityCurve::pointer GetWorkingTAC() const { return working_tac_; }
    syd::FitModelBase::pointer GetFitModel() const { return current_best_model_; }
    int GetNbOfIterations() const { return nb_of_iterations; }
    syd::FitModelBase::vector GetModels() const { return models_; }

  protected:
    /// Input
    syd::TimeActivityCurve::pointer initial_tac_;
    syd::TimeActivityCurve::pointer working_tac_;
    syd::TimeIntegratedActivityFitOptions options_;

    syd::FitModelBase::vector models_;
    syd::FitModelBase::pointer current_best_model_;
    int nb_of_iterations;

    //std::vector<double> additional_point_times;
    //std::vector<double> additional_point_values;

    /// Options for the solver
    ceres::Solver::Options * ceres_options_;
    ceres::Solver::Summary ceres_summary_;
    ceres::Solver::Summary current_ceres_summary_;

    /// Initialize the solver
    void InitSolver();

    /// Check the inputs (size etc)
    virtual void CheckInputs();

    /// Try to fit a TAC with the given model
    void FitTACWithModel(syd::FitModelBase::pointer model,
                         syd::TimeActivityCurve::pointer tac);

    /// Choose the best lmode according to Akaike criterion
    int SelectBestModel(syd::FitModelBase::vector models,
                        syd::TimeActivityCurve::pointer tac);

    /// Compute the restricted tac from the max (3 points at min)
    int GetRestrictedTac(syd::TimeActivityCurve::pointer initial_tac,
                         syd::TimeActivityCurve::pointer restricted_tac);


  }; // class TimeIntegratedActivityFilter

} // namespace syd
// --------------------------------------------------------------------

#endif
