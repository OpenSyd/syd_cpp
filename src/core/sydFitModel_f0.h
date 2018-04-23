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

#ifndef SYDFITMODEL_F0_H
#define SYDFITMODEL_F0_H

// syd
#include "sydFitModelBase.h"

// --------------------------------------------------------------------
namespace syd {

  class FitModel_f0: public FitModelBase
  {
  public:

    FitModel_f0();
    ~FitModel_f0() {}

    class ResidualType: public FitModelBase::ResidualBaseType {
    public:
      ResidualType(double x, double y, double l):ResidualBaseType(x,y,l) {}
      // template required for ceres
      template <typename T> bool operator()(const T* const A, T* residual) const;
    };

    typedef ceres::AutoDiffCostFunction<ResidualType, 1, 1> CostFctType;

    virtual int GetNumberOfExpo() const { return 1; }
    void SetA(double i);
    virtual double GetA(const int A) const;
    virtual double GetLambda(const int i) const;

    virtual FitModelBase * Clone() const;
    virtual void SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac);
    virtual double GetValue(const double & time) const;

    virtual void Scale(double s);

    virtual void ComputeStartingParametersValues(const syd::TimeActivityCurve::pointer tac);

  protected:
    std::vector<ResidualType*> residuals_;
    double initialActivity_;

  };

}  // namespace syd


// --------------------------------------------------------------------

#endif
