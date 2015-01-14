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

// --------------------------------------------------------------------
syd::TimeActivityCurveFitSolver::TimeActivityCurveFitSolver()
{
  tac_ = NULL;
  useWeightedFit_ = true;
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
void syd::TimeActivityCurveFitSolver::Run()
{
  // DD("TimeActivityCurveFitSolver::Run");
  if (tac_ == NULL) {
    LOG(FATAL) << "Error in TimeActivityCurveFitSolver, use SetInput(tac) first";
  }
  unsigned int kNumObservations = tac_->size();
  // DD(kNumObservations);

  // get max value ?
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
  // problem.SetParameterLowerBound(&lambda, 0, Lambda_Indium*0.99);
  // problem.SetParameterUpperBound(&lambda, 0, Lambda_Indium*1.01);

  // Solve
  ceres::Solver::Options options;
  options.max_num_iterations = 100;
  options.linear_solver_type = ceres::DENSE_QR;
  options.minimizer_progress_to_stdout = false;
  options.trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT; // LM is the default
  // may try Dogleg also

  // Set comment FIXME

  // Set rms FIXME

  // output
  ceres::Solver::Summary summary;
  Solve(options, &problem, &summary);
  // std::cout << summary.BriefReport() << "\n";
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
