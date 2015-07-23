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
#include "sydFitModelBase.h"

// --------------------------------------------------------------------
syd::FitModelBase::FitModelBase()
{
  name_ = "no_name";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModelBase::SetProblemResidual(ceres::Problem * problem, syd::TimeActivityCurve & tac)
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitModelBase::CopyFrom(const syd::FitModelBase * model)
{
  DD("copy");
  name_ = model->GetName();
  lambda_phys_hours_ = model->GetLambdaPhysicHours();
  params_  = model->GetParameters();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeActivityCurve * syd::FitModelBase::GetTAC(double first_time, double last_time, int n) const
{
  syd::TimeActivityCurve * tac = new syd::TimeActivityCurve;
  double step = (last_time-first_time)/(double)n;
  double time = first_time;
  for(auto i=0; i<n; i++) {
    tac->AddValue(time, GetValue(time));
    time += step;
  }
  return tac;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
namespace syd {
  std::ostream& operator<<(std::ostream& os, const FitModelBase & p)
  {

  }
} // end namespace
// --------------------------------------------------------------------
