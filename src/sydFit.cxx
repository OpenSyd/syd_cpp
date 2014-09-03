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

#include "sydFit.h"
#include "syd_With_Verbose_Flag.h"

// --------------------------------------------------------------------
struct syd::MonoExponentialResidualWeighted {
  MonoExponentialResidualWeighted(double x, double y, double w): x_(x), y_(y), w_(w) {}
  template <typename T> bool operator()(const T* const A,
                                        const T* const lambda,
                                        T* residual) const {
    residual[0] = w_ * (T(y_) - A[0]*exp(-lambda[0] * T(x_)));
    return true;

  }
private:
  const double x_;
  const double y_;
  const double w_;
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
struct syd::MonoExponentialResidual {
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
struct syd::BiExponentialResidual {
  BiExponentialResidual(double x, double y): x_(x), y_(y) {}
  template <typename T> bool operator()(const T* const A,
                                        const T* const lambda,
                                        const T* const B,
                                        const T* const lambda2,
                                        T* residual) const {
    residual[0] = T(y_) - (A[0]*exp(-lambda[0] * T(x_)) + B[0]*exp(-lambda2[0] * T(x_)));
    //    residual[0] = T(y_) - A[0]*(B[0]*exp(-lambda[0] * T(x_)) + (1.0-B[0])*exp(-lambda2[0] * T(x_)));

    residual[0] = T(y_) - (
                           A[0]*B[0]*exp(-lambda[0] * T(x_)) +
                           A[0]*(1.0-B[0])*exp(-lambda2[0] * T(x_)));

    return true;

  }
private:
  const double x_;
  const double y_;
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
struct syd::BiExponentialResidual2 {
  BiExponentialResidual2(double x, double y): x_(x), y_(y) {}
  template <typename T> bool operator()(const T* const A,
                                        const T* const lambda,
                                        const T* const lambda2,
                                        T* residual) const {
    // residual[0] = T(y_) -
    //   ((lambda[0]*lambda2[0])/(A[0]*(lambda2[0]-lambda[0]))) *
    //   (exp(-lambda[0]*T(x_)) - exp(-lambda2[0]*T(x_)));

    residual[0] = T(y_) -
      (A[0]*exp(-lambda[0] * T(x_)) +
       (A[0]*lambda[0]/(lambda[0]-lambda2[0]))*(exp(-lambda2[0]*T(x_)) - exp(-lambda[0]*T(x_))));
    return true;

  }
private:
  const double x_;
  const double y_;
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
struct syd::BiExponentialResidual3 {
  BiExponentialResidual3(double x, double y): x_(x), y_(y) {}
  template <typename T> bool operator()(const T* const A,
                                        const T* const lambda,
                                        const T* const lambda2,
                                        T* residual) const {
    // residual[0] = T(y_) -
    //   ((lambda[0]*lambda2[0])/(A[0]*(lambda2[0]-lambda[0]))) *
    //   (exp(-lambda[0]*T(x_)) - exp(-lambda2[0]*T(x_)));

    // residual[0] = T(y_) -
    //   (A[0]*exp(-lambda[0] * T(x_)) +
    //    (A[0]*lambda[0]/(lambda[0]-lambda2[0]))*(exp(-lambda2[0]*T(x_)) - exp(-lambda[0]*T(x_))));

    // residual[0] = T(y_) -
    //   (

    return true;

  }
private:
  const double x_;
  const double y_;
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Fit(std::vector<double> & times, std::vector<double> & activities)
{
  std::vector<double> variances(times.size());
  std::fill(variances.begin(), variances.end(), 0.0);
  Fit(times, activities, variances);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Fit(std::vector<double> & times, std::vector<double> & activities,
              std::vector<double> & variances) {


}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitTest(std::vector<double> & times,
              std::vector<double> & activities,
              std::vector<double> & variances) {

  // Param initialisation
  double A = activities[1]; // not zero
  double lambda = Lambda_Indium;
  double B = 0.5;//activities[0];
  double lambda2 = Lambda_Indium/2.0;
  DD(A);
  DD(lambda);
  DD(B);
  DD(lambda2);

  int kNumObservations = times.size();
  DD(kNumObservations);

  // // Weight according to variance
  // double w = 0.0;
  // for(auto i=0; i<kNumObservations; i++) {
  //   activities[i] = activities[i]/variances[i];
  //   w += 1.0/variances[i];
  // }
  // DD(w);

  // typedef to short the code
  // 1,1,1 ==> dimension of residual, dimension of parameters
  typedef ceres::AutoDiffCostFunction<MonoExponentialResidual, 1, 1, 1> CostFctType2;
  typedef ceres::AutoDiffCostFunction<BiExponentialResidual3, 1, 1, 1, 1> CostFctType3;
  typedef ceres::AutoDiffCostFunction<BiExponentialResidual, 1, 1, 1, 1, 1> CostFctType4;

  // declare obj function
  ceres::Problem problem;
  for (int i = 0; i < kNumObservations; ++i) {
    /*problem.AddResidualBlock(new CostFctType2(new MonoExponentialResidual(times[i], activities[i], variances[i])),
      NULL,
      &A, &lambda);
    */
    problem.AddResidualBlock(new CostFctType4(new BiExponentialResidual(times[i], activities[i])),
                             NULL,
                             &A, &lambda, &B, &lambda2);

    /*problem.AddResidualBlock(new CostFctType3(new BiExponentialResidual3(times[i], activities[i])),
                             NULL,
                             &A, &lambda, &lambda2);*/
  }

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

  // output
  ceres::Solver::Summary summary;
  Solve(options, &problem, &summary);
  std::cout << summary.BriefReport() << "\n";

  // for gp
  DD(Lambda_Indium);
  std::cout << "A = " << A << std::endl
            << "lambda = " << lambda << std::endl
            << "B = " << B << std::endl
            << "lambda2 = " << lambda2 << std::endl
    ;

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Fit_Time_Activity::Fit_Time_Activity():With_Verbose_Flag()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Fit_Time_Activity::
Set_Data(const std::vector<double> & _times,
         const std::vector<double> & _activities,
         const std::vector<double> & _std)
{
  times = &_times;
  activities = &_activities;
  std = &_std;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Fit_Time_Activity::
Fit_With_Mono_Expo()
{
  int kNumObservations = times->size();
  DD(kNumObservations);
  m_Parameters.resize(2);
  double & A = m_Parameters[0];
  double & lambda = m_Parameters[1];
  A = (*activities)[1];
  lambda = Lambda_Indium;
  DD(A);
  DD(lambda);

  // 1,1 ==> dimension of residual, dimension of parameters
  typedef ceres::AutoDiffCostFunction<MonoExponentialResidual, 1, 1, 1> CostFctType;

  // Declare the problem with residual blocks
  ceres::Problem problem;
  for (int i = 0; i < kNumObservations; ++i) {
    problem.AddResidualBlock(new CostFctType(new MonoExponentialResidual((*times)[i], (*activities)[i])),
                             NULL,
                             &A, &lambda);
  }

  // Solve
  ceres::Solver::Options options;
  options.max_num_iterations = 100;
  options.linear_solver_type = ceres::DENSE_QR;
  options.minimizer_progress_to_stdout = false;
  options.trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT; // LM is the default
  options.logging_type = ceres::SILENT;
  // may try Dogleg also

  // Get the results
  DD("here");
  ceres::Solver::Summary summary;
  Solve(options, &problem, &summary);
  //  std::cout << summary.BriefReport() << "\n";
  DD(A);
  DD(lambda);

  m_RMS = -1; // FIXME
}
// --------------------------------------------------------------------
