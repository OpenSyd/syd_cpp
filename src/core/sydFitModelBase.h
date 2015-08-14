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

// syd
#include "sydTimeActivityCurve.h"

// std
#include <vector>

// --------------------------------------------------------------------
namespace syd {

  // This is needed only once in the main to ensure that solver is SILENT
#define SYD_CERES_STATIC_INIT google::InitGoogleLogging("");

  class FitModelBase
  {
  public:

    FitModelBase();
    ~FitModelBase() {}

    class ResidualBaseType {
    public:
      ResidualBaseType(double x, double y, double l): x_(x), y_(y), lambda(l) {}
      const double x_;
      const double y_;
      const double lambda;
    };

    int id_;
    double robust_scaling_;

    virtual std::string GetName() const { return name_; }
    //int GetNumberOfParameters() const { return params_.size(); }
    int GetK()  const { return params_.size(); } //const { return GetNumberOfParameters(); } // Ka
    virtual int GetNumberOfExpo() const = 0;//; { return GetNumberOfParameters(); } // Ka

    void SetLambdaPhysicHours(double l) { lambda_phys_hours_ = l; }

    double GetLambdaPhysicHours() const { return lambda_phys_hours_; }
    std::vector<double> & GetParameters() { return params_; }
    const std::vector<double> & GetParameters() const { return params_; }

    virtual FitModelBase * Clone() const = 0;
    virtual void SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac);
    virtual syd::TimeActivityCurve * GetTAC(double first_time, double last_time, int n) const;

    virtual double GetValue(const double & time) const = 0;

    void CopyFrom(const syd::FitModelBase * model);

    friend std::ostream& operator<<(std::ostream& os, const FitModelBase & p);

    double Integrate(double a, double b) const;
    double Integrate() const;

    double ComputeAUC(const syd::TimeActivityCurve & tac, bool use_current_tac=false) const;
    double ComputeR2(const syd::TimeActivityCurve & tac, bool use_current_tac=false) const;
    double ComputeAICc(const syd::TimeActivityCurve & tac) const;
    double ComputeSS(const syd::TimeActivityCurve & tac) const;
    bool IsAICcValid(int N) const;
    virtual bool IsAcceptable() const;

    virtual double GetA(const int i) const { LOG(sydlog::FATAL) << "GetA to implement " << GetName(); return 0.0; }
    virtual double GetLambda(const int i) const { LOG(sydlog::FATAL) << "GetLambda to implement " << GetName(); return 0.0; }

    bool start_from_max_flag;

    // protected:
    std::string name_;
    double lambda_phys_hours_;
    std::vector<double> params_;
    syd::TimeActivityCurve * current_tac;
    double current_starting_time;
    ceres::Solver::Summary ceres_summary_;

  };

}  // namespace syd

// --------------------------------------------------------------------

#endif
