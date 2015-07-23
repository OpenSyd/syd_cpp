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

// std
#include <vector>

// --------------------------------------------------------------------
namespace syd {

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


    virtual std::string GetName() const { return name_; }
    int GetNumberOfParameters() const { return params_.size(); } // K

    void SetLambdaPhysicHours(double l) { lambda_phys_hours_ = l; }

    double GetLambdaPhysicHours() const { return lambda_phys_hours_; }
    const std::vector<double> & GetParameters() const { return params_; }

    virtual FitModelBase * Clone() const = 0;
    virtual void SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac);
    virtual syd::TimeActivityCurve * GetTAC(double first_time, double last_time, int n) const;

    virtual double GetValue(const double & time) const = 0;

    void CopyFrom(const syd::FitModelBase * model);

    friend std::ostream& operator<<(std::ostream& os, const FitModelBase & p);

  protected:
    std::string name_;
    double lambda_phys_hours_;
    std::vector<double> params_;
  };

}  // namespace syd

// --------------------------------------------------------------------

#endif
