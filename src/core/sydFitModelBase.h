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

#ifndef SYDFITMODELBASE_H
#define SYDFITMODELBASE_H

// ceres
#include "ceres/ceres.h"

// google logging (ceres)
#include <glog/logging.h>
#undef LOG

// Reset log macro. There is a macro name conflict because glog/ceres
// use a macro named LOG, so we need to reput syd LOG macro here
#include "sydLogMacro.h"

// syd
#include "sydTimeActivityCurve.h"

// std
#include <vector>

// --------------------------------------------------------------------
namespace syd {

  // This is needed only once in the main to ensure that solver is SILENT
#define SYD_CERES_STATIC_INIT google::InitGoogleLogging("");


  //FIXME to clean

  // Base class for a fit model (multi exponential)
  class FitModelBase {
  public:

    // Types (no 'New' because the class is abstract)
    typedef std::shared_ptr<FitModelBase> pointer;
    typedef std::vector<pointer> vector;

    // Constructors
    FitModelBase();
    virtual ~FitModelBase() {}

    int GetId() { return id_; }
    int GetK()  const { return params_.size(); }
    ceres::Solver::Summary & GetSummary() { return ceres_summary_; }
    double GetLambdaDecayConstantInHours() const { return lambda_in_hours_; }
    std::vector<double> & GetParameters() { return params_; }
    const std::vector<double> & GetParameters() const { return params_; }
    syd::TimeActivityCurve::pointer GetTAC(const std::vector<double> & times) const;
    virtual double GetLambda(const int i) const { LOG(FATAL) << "GetLambda to implement " << GetName(); return 0.0; }
    virtual syd::TimeActivityCurve::pointer GetTAC(double first_time, double last_time, int n) const;
    virtual std::string GetName() const { return name_; }
    virtual double GetValue(const double & time) const = 0;
    virtual double GetA(const int i) const { LOG(FATAL) << "GetA to implement " << GetName(); return 0.0; }
    virtual double GetEffHalfLife() const;
    virtual int GetNumberOfExpo() const = 0;

    void SetLambdaDecayConstantInHours(double l) { lambda_in_hours_ = l; }
    void SetParameters(const std::vector<double> & p);
    virtual void SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac);

    virtual void Scale(double s) = 0;
    virtual FitModelBase * Clone() const = 0;
    void CopyFrom(const syd::FitModelBase * model);

    friend std::ostream& operator<<(std::ostream& os, const FitModelBase & p);

    double Integrate(double a, double b) const;
    double Integrate() const;

    double ComputeAUC_OLD(const syd::TimeActivityCurve & tac, bool use_current_tac=false) const;
    double ComputeAUC(const syd::TimeActivityCurve::pointer tac, int index) const;
    double ComputeR2(const syd::TimeActivityCurve::pointer tac) const;
    double ComputeAICc(const syd::TimeActivityCurve::pointer tac) const;
    double ComputeAIC(const syd::TimeActivityCurve::pointer tac) const;
    double ComputeRSS(const syd::TimeActivityCurve::pointer tac) const;
    virtual void ComputeStartingParametersValues(const syd::TimeActivityCurve::pointer tac) {
      DD("ComputeStartingParametersValues not implemented ");
    }
    bool IsAICcValid(int N) const;
    virtual bool IsAcceptable() const;

    void LogLinearFit(Eigen::Vector2d & x,
                      const syd::TimeActivityCurve::pointer tac,
                      int start=0, int end=-1);

    // Nested class to store residual
    class ResidualBaseType {
    public:
      ResidualBaseType(double x, double y, double l): x_(x), y_(y), lambda(l) {}
      const double x_;
      const double y_;
      const double lambda;
    };

  protected:
    // protected:
    int id_;
    std::string name_;
    double lambda_in_hours_;
    std::vector<double> params_;
    syd::TimeActivityCurve * current_tac;
    double current_starting_time;
    ceres::Solver::Summary ceres_summary_;
    bool start_from_max_flag;
    double robust_scaling_;


  };

}  // namespace syd

// --------------------------------------------------------------------

#endif
