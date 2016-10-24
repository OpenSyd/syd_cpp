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
#include "sydTimeIntegratedActivityFilter.h"
#include "sydImageUtils.h"

// itk
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>

// --------------------------------------------------------------------
syd::TimeIntegratedActivityFilter::TimeIntegratedActivityFilter()
{
  DDF();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeIntegratedActivityFilter::~TimeIntegratedActivityFilter()
{
  delete ceres_options_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFilter::Run()
{
  DDF();

  // Check inputs: size, times, negative values ? lambda
  CheckInputs();

  // Initialisation TAC
  // working_tac_ = syd::TimeActivityCurve::New();
  //  DD("ADD value here");//FIXME

  // Init models
  models_ = options_->GetModels();

  // Initialisation: Solver
  InitSolver();

  // Create working tac (restricted, + add value)
  if (options_.GetRestrictedFlag()) GetRestrictedTac(initial_tac_, working_tac_);
  else working_tac_ = initial_tac_;
  DD(initial_tac_);
  DD(working_tac_);

  // Loop on models
  for(auto model:models_) {
    FitTACWithModel(model, working_tac_);
  }

  // Select best one
  auto best_model = SelectBestModel(models_, working_tac_);
  DD(best_model);
  if (best_model > 0) current_best_model_ = models[best_model];
  else current_best_model_ = nullptr;

  if (current_best_model_ != nullptr) {
    auto summary = current_best_model_->GetSummary();
    nb_of_iterations = summary.num_unsuccessful_steps + summary.num_successful_steps;
  }
  else nb_of_iterations = 0;
  DD(nb_of_iterations);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::TimeIntegratedActivityFilter::
GetRestrictedTac(syd::TimeActivityCurve::pointer tac,
                 syd::TimeActivityCurve::pointer restricted_tac)
{
  restricted_tac->clear();
  // Select only the end of the curve from the largest value find from
  // the end
  int i = tac->FindIndexOfMaxValueFromTheEnd(3);
  for(int j=i; j<tac->size(); j++)
    restricted_tac->AddValue(tac->GetTime(j), tac->GetValue(j));
  return i;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFilter::InitSolver()
{
  // Solve
  ceres_options_ = new ceres::Solver::Options;
  ceres_options_->max_num_iterations = options_.GetMaxNumIterations();
  ceres_options_->linear_solver_type = ceres::DENSE_QR; // because few parameters/data
  //  ceres_options_->linear_solver_type = ceres::DENSE_SCHUR;
  ceres_options_->minimizer_progress_to_stdout = false;
  ceres_options_->trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT; // LM is the default

  /*
    DD(ceres_options_->min_line_search_step_size);
    DD(ceres_options_->max_line_search_step_contraction);
    DD(ceres_options_->function_tolerance);
    DD(ceres_options_->parameter_tolerance);
    DD(ceres_options_->num_threads);
  */

  //ceres_options_->function_tolerance = 1e-8;

  //ceres_options_->trust_region_strategy_type = ceres::DOGLEG;// (LM seems faster)
  //ceres_options_->dogleg_type = ceres::SUBSPACE_DOGLEG;
  ceres_options_->logging_type = ceres::SILENT;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFilter::CheckInputs()
{
  if (tac_.GetSize() < 2) {
    EXCEPTION("Provide at least 2 timepoints");
  }
  if (options_->GetLambdaDecayConstantInHours() == 0.0) {
    EXCEPTION("You must provide the decay constant (lambda in hours)");
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFilter::
FitTACWithModel(syd::FitModelBase::pointer model,
                syd::TimeActivityCurve::pointer tac)
{
  ceres::Problem problem;// New problem each time. (I did not manage to change that)
  model->ComputeStartingParametersValues(tac);
  model->SetProblemResidual(&problem, *tac);
  ceres::Solve(*ceres_options_, &problem, &model->GetSummary()); // Go !
  //    DD(model->GetSummary().FullReport());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::TimeIntegratedActivityFilter::
SelectBestModel(syd::FitModelBase::vector models,
                syd::TimeActivityCurve::pointer tac)
{
  bool verbose=false; // Debug
  int best = -1;
  double R2_threshold = options_.GetR2MinThreshold();
  double min_Akaike_criterion = 666.0;
  double best_R2 = 0.0;
  if (verbose) {
    std::cout << initial_tac_ << std::endl;
    std::cout << working_tac_ << std::endl;
  }

  for(auto i=0; i<models.size(); i++) {
    auto & m = models[i];
    double R2 = m->ComputeR2(tac);
    if (verbose) std::cout << m->GetName()
                           << " SS = " << m->ComputeRSS(tac)
                           << " R2 = " << R2;
    if (R2 > R2_threshold) {
      double criterion;
      if (options_.GetAkaikeCriterion() == "AIC") {
        criterion = m->ComputeAIC(tac);
      } else {
        if (options_.GetAkaikeCriterion() == "AICc") {
          criterion = m->ComputeAICc(tac);
        } else {
          LOG(FATAL) << "Akaike criterion '"
                     << options_.GetAkaikeCriterion() << "' not known"
                     << ". Use AIC or AICc";
        }
      }

      if (verbose) std::cout << " valid=" <<  m->IsAICcValid(tac->size())
                             << " AICc = " << m->ComputeAICc(tac)
                             << " AIC = " << m->ComputeAIC(tac) << std::endl;
      if (criterion < min_Akaike_criterion) {
        best = i;
        min_Akaike_criterion = criterion;
        best_R2 = R2;
      }
    }
    else if (verbose) std::cout << std::endl;
  }
  if (verbose) std::cout << best << std::endl;
  return best;
}
// --------------------------------------------------------------------


