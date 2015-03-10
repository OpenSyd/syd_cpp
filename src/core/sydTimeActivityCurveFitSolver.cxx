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
#include "sydTimeActivityCurveFitSolver.h"

// std
#include <limits>
#include <cmath>
#include <cfloat>

// --------------------------------------------------------------------
syd::TimeActivityCurveFitSolver::TimeActivityCurveFitSolver()
{
  tac_ = NULL;
  useWeightedFit_ = true;
  ceres_options_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeActivityCurveFitSolver::~TimeActivityCurveFitSolver()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurveFitSolver::SetInput(TimeActivityCurve * tac)
{
  tac_ = tac;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::TimeActivityCurveFitSolver::GetFitNbOfIterations() const
{
  return ceres_summary_.num_successful_steps;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
struct syd::TimeActivityCurveFitSolver::MonoExponentialResidual {
  MonoExponentialResidual(double x, double y): x_(x), y_(y) {}
  template <typename T> bool operator()(const T* const A,
                                        const T* const lambda,
                                        T* residual) const {
    residual[0] = (T(y_) - A[0]*exp(-lambda[0] * T(x_)));
    return true;

  }
private:
  const double x_;
  const double y_;
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
struct syd::TimeActivityCurveFitSolver::MonoExponentialResidualWeighted {
  MonoExponentialResidualWeighted(double x, double y, double w): x_(x), y_(y), w_(w) {}
  template <typename T> bool operator()(const T* const A,
                                        const T* const lambda,
                                        T* residual) const {
    residual[0] = 1.0/w_ * (T(y_) - A[0]*exp(-lambda[0] * T(x_)));
    return true;
  }
private:
  const double x_;
  const double y_;
  const double w_;
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::TwoExponential(const double A, const double K1, const double K2, const double x)
{
  return A*K1/(K2-K1)*exp(-syd::Lambda_Indium_in_hours*x)*( exp(-K1*x) - exp(-K2*x) );
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
struct syd::TimeActivityCurveFitSolver::TwoExponentialResidual {
  TwoExponentialResidual(double x, double y): x_(x), y_(y) {}
  template <typename T> bool operator()(const T* const vA,
                                        const T* const vK1,
                                        const T* const vK2,
                                        T* residual) const {
    const T y = T(y_);
    const T x = T(x_);
    const T A = vA[0];
    const T K1 = vK1[0];
    const T K2 = vK2[0];

    /* f4a
       residual[0] = (y - (A * K1) / (K2-K1) *
       exp(-(Lambda_Indium_in_hours*x)) *
       (exp(-K1*x) - exp(-K2*x)));
    */

    // f4b
    /*residual[0] = (y -
                   (A * exp(-(K1+Lambda_Indium_in_hours)*x) +
                   (100.0-A) * exp(-(K2+Lambda_Indium_in_hours)*x)));*/

    // f3
    residual[0] = (y -
                   (A * exp(-(K2+Lambda_Indium_in_hours)*x) +
                    K1 * exp(-Lambda_Indium_in_hours*x)));

    // f4c
    /*residual[0] = (y -
                   100.0 * (A)/(K1+A-K2)
                   * (exp(-(Lambda_Indium_in_hours+K2)*x) - exp(-(Lambda_Indium_in_hours+A+K1)*x)));*/

    return true;
  }
private:
  const double x_;
  const double y_;
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurveFitSolver::Run()
{
  // DD("TimeActivityCurveFitSolver::Run");
  if (tac_ == NULL) {
    LOG(FATAL) << "Error in TimeActivityCurveFitSolver, use SetInput(tac) first";
  }
  unsigned int kNumObservations = tac_->size();
  // DD(kNumObservations);

  // get max value ? // FIXME to chanbge by calling the function FindMaxIndex
  double max=0.0;
  int max_index;
  for (auto i = 0; i < kNumObservations; ++i) { // FIXME start at 0 or 1 ?
    if (tac_->GetValue(i) > max) {
      max = tac_->GetValue(i);
      max_index = i;
    }
  }
  // DD(max_index);

  // Declare obj function
  typedef ceres::AutoDiffCostFunction<MonoExponentialResidual, 1, 1, 1> CostFctType1;
  typedef ceres::AutoDiffCostFunction<MonoExponentialResidualWeighted, 1, 1, 1> CostFctType2;
  std::vector<double> val;
  std::vector<double> times;
  ceres::Problem problem;

  if (useWeightedFit_) {
    std::vector<MonoExponentialResidualWeighted*> residuals;
    for (auto i=max_index; i<kNumObservations; ++i) {
      auto r = new MonoExponentialResidualWeighted(tac_->GetTime(i), tac_->GetValue(i), tac_->GetVariance(i));
      val.push_back(tac_->GetValue(i));
      times.push_back(tac_->GetTime(i));
      residuals.push_back(r);
    }
    // Update the nb of observations according to starting point (max)
    kNumObservations = residuals.size();
    // Build the problem
    for (auto i=0; i<kNumObservations; ++i) {
      problem.AddResidualBlock(new CostFctType2(residuals[i]), NULL, &A_, &lambda_);
    }
  }
  else {
    std::vector<MonoExponentialResidual*> residuals;
    for (auto i=max_index; i<kNumObservations; ++i) {
      auto r = new MonoExponentialResidual(tac_->GetTime(i), tac_->GetValue(i));
      val.push_back(tac_->GetValue(i));
      times.push_back(tac_->GetTime(i));
      residuals.push_back(r);
    }
    // Update the nb of observations according to starting point (max)
    kNumObservations = residuals.size();
    // Build the problem
    for (auto i=0; i<kNumObservations; ++i) {
      problem.AddResidualBlock(new CostFctType1(residuals[i]), NULL, &A_, &lambda_);
    }
  }

  // init  needed ?
  A_ = tac_->GetValue(0);
  nb_used_points_ = kNumObservations;

  // Bounds (constraints)
  // problem.SetParameterLowerBound(&A, 0, 0); // A positive
  // // problem.SetParameterLowerBound(&B, 0, 0); // B positive
  // // //  problem.SetParameterUpperBound(&B, 0, 1); // B <=1.0
  // problem.SetParameterLowerBound(&lambda, 0, 0); // positive
  // problem.SetParameterLowerBound(&lambda2, 0, 0); // positive
  // problem.SetParameterLowerBound(&lambda_, 0, Lambda_Indium_in_hours*0.99);
  // problem.SetParameterUpperBound(&lambda_, 0, Lambda_Indium_in_hours*1.01);

  // Solve
  ceres::Solver::Options options;
  options.max_num_iterations = 100;
  options.linear_solver_type = ceres::DENSE_QR;
  options.minimizer_progress_to_stdout = false;
  options.trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT; // LM is the default
  // may try Dogleg also
  options.logging_type = ceres::SILENT;

  // Set comment FIXME

  // Set rms FIXME

  // output
  ceres::Solver::Summary summary;
  Solve(options, &problem, &summary);
  //std::cout << summary.BriefReport() << "\n";
  // std::cout << summary.FullReport() << "\n";

  /*
    bool Solver::Summary::IsSolutionUsable() const
    double Solver::Summary::final_cost -> is half the rms ???
  */

  double rms =0.0;
  for(auto i=0; i<val.size(); i++) {
    double m = A_*exp(-lambda_*times[i]);
    rms += pow(m - val[i], 2);
  }
  // DD(rms);
  // DD(rms/2.0);
  // DD(summary.final_cost);
  //  fit_error_ = summary.final_cost;
  fit_error_ = rms;//summary.final_cost;

  // DD(A_);
  // DD(lambda_);

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurveFitSolver::IncrementalRun()
{
  // Get the index of the max value in the curve
  unsigned int max_index = tac_->FindMaxIndex();

  // Fill the residuals (needed because the number can vary)
  residuals_.clear();
  for (auto i=max_index; i<tac_->size(); ++i) {
    auto r = new MonoExponentialResidual(tac_->GetTime(i), tac_->GetValue(i));
    residuals_.push_back(r);
  }

  // Build the problem
  ceres::Problem problem;
  for (auto i=0; i<residuals_.size(); ++i) {
    problem.AddResidualBlock(new CostFctType1(residuals_[i]), NULL, &A_, &lambda_);
  }

  // Init  (needed  ?)
  A_ = tac_->GetValue(0);
  nb_used_points_ = residuals_.size();

  // Bounds (constraints)
  //problem.SetParameterLowerBound(&A_, 0, 0); // A positive
  //  problem.SetParameterLowerBound(&B, 0, 0); // B positive
  // // //  problem.SetParameterUpperBound(&B, 0, 1); // B <=1.0
  //problem.SetParameterLowerBound(&lambda_, 0, 0); // positive
  // problem.SetParameterLowerBound(&lambda2, 0, 0); // positive
  // problem.SetParameterLowerBound(&lambda, 0, Lambda_Indium*0.99);
  // problem.SetParameterUpperBound(&lambda, 0, Lambda_Indium*1.01);
  problem.SetParameterLowerBound(&lambda_, 0, Lambda_Indium_in_hours*0.2);
  //  problem.SetParameterUpperBound(&lambda_, 0, Lambda_Indium_in_hours*1.5);

  // output
  Solve(*ceres_options_, &problem, &ceres_summary_);
  //std::cout << summary.BriefReport() << "\n";
  //std::cout << summary.FullReport() << "\n";

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurveFitSolver::InitIncrementalRun()
{
  if (ceres_options_) delete ceres_options_;
  ceres_options_ = new ceres::Solver::Options;
  ceres_options_->max_num_iterations = 500;
  ceres_options_->linear_solver_type = ceres::DENSE_QR;
  ceres_options_->minimizer_progress_to_stdout = false;
  ceres_options_->trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT; // LM is the default
  //ceres_options_->trust_region_strategy_type = ceres::DOGLEG;// (faster LM ?)
  //ceres_options_->dogleg_type = ceres::SUBSPACE_DOGLEG;
  ceres_options_->logging_type = ceres::SILENT;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeActivityCurveFitSolver::Run_f4a()
{
  DD("Run_f4a");
  std::vector<TwoExponentialResidual*> residuals;
  typedef ceres::AutoDiffCostFunction<TwoExponentialResidual, 1, 1, 1, 1> CostFctType;

  // Fill the residuals
  residuals.clear();
  for (auto i=0; i<tac_->size(); ++i) {
    auto r = new TwoExponentialResidual(tac_->GetTime(i), tac_->GetValue(i));
    residuals.push_back(r);
  }

  // Build the problem
  unsigned int max_index = tac_->FindMaxIndex();
  A_ = -0.1;//2*tac_->GetValue(max_index); // required ! (or could be index=0)
  K1_ = 0.01;//tac_->GetValue(max_index);
  K2_ = 0.001;
  ceres::Problem problem;
  for (auto i=0; i<residuals.size(); ++i) {
    problem.AddResidualBlock(new CostFctType(residuals[i]), NULL, &A_, &K1_, &K2_);
  }

  // Bounds (constraints)
  // problem.SetParameterLowerBound(&K1_, 0, 0); // A positive
  problem.SetParameterLowerBound(&K2_, 0, 0); // B positive
  //  problem.SetParameterUpperBound(&K1_, 0, 0.05); // B <=1.0
  //  problem.SetParameterUpperBound(&K2_, 0, 0.05); // positive
  // problem.SetParameterLowerBound(&lambda2, 0, 0); // positive
  //  problem.SetParameterLowerBound(&K2, 0, Lambda_Indium*0.99);
  // problem.SetParameterUpperBound(&lambda, 0, Lambda_Indium*1.01);
  //problem.SetParameterLowerBound(&lambda_, 0, Lambda_Indium_in_hours*0.2);
  //  problem.SetParameterUpperBound(&lambda_, 0, Lambda_Indium_in_hours*1.5);

  // output
  Solve(*ceres_options_, &problem, &ceres_summary_);
  std::cout << ceres_summary_.BriefReport() << "\n";
  // std::cout << ceres_summary_.FullReport() << "\n";
}
// --------------------------------------------------------------------
