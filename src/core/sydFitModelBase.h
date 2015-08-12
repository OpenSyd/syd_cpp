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

// syd
#include "sydTimeActivityCurve.h"

// ceres
#include "ceres/ceres.h"

// google logging (ceres)
#include <glog/logging.h>

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

    double ComputeAUC() const;
    double ComputeR2(syd::TimeActivityCurve & tac) const;
    double ComputeAICc(syd::TimeActivityCurve & tac) const;
    double ComputeSS(syd::TimeActivityCurve & tac) const;

    virtual double GetA(const int i) const { LOG(FATAL) << "GetA to implement " << GetName(); }
    virtual double GetLambda(const int i) const { LOG(FATAL) << "GetLambda to implement " << GetName(); }

    bool start_from_max_flag;

    // protected:
    std::string name_;
    double lambda_phys_hours_;
    std::vector<double> params_;
    syd::TimeActivityCurve * current_tac;

  };

}  // namespace syd

// --------------------------------------------------------------------

#endif
