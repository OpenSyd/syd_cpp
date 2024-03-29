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

#ifndef SYDFITMODEL_F4A_H
#define SYDFITMODEL_F4A_H

// syd
#include "sydFitModelBase.h"

// --------------------------------------------------------------------
namespace syd {

  class FitModel_f4a: public FitModelBase
  {
  public:

    FitModel_f4a();
    virtual ~FitModel_f4a() {}

    class ResidualType: public FitModelBase::ResidualBaseType {
    public:
      ResidualType(double x, double y, double l):ResidualBaseType(x,y,l) {}
      // template required for ceres
      template <typename T> bool operator()(const T* const A,
                                            const T* const l1,
                                            const T* const l2,
                                            T* residual) const;
    };

    // 1, 1, 1 ==> (nb of residuals=1), 1 for each param.
    typedef ceres::AutoDiffCostFunction<ResidualType, 1, 1, 1, 1> CostFctType;

    void ComputeStartingParametersValues(const syd::TimeActivityCurve::pointer tac);

    virtual int GetNumberOfExpo() const { return 2; }
    virtual double GetA(const int i) const;
    virtual double GetLambda(const int i) const;

    virtual FitModelBase * Clone() const;
    virtual void SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac);
    virtual double GetValue(const double & time) const;
    virtual bool IsAcceptable() const;
    virtual void Scale(double s);

  protected:
    std::vector<ResidualType*> residuals_;

  };

}  // namespace syd


// --------------------------------------------------------------------

#endif
