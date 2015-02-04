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
#include "sydActivityTimeIntegratedCommand.h"
#include "sydImage.h"
#include "sydActivityLambdaCommand.h"
#include "sydTimeActivityCurveIntegrate.h"

// --------------------------------------------------------------------
syd::ActivityTimeIntegratedCommand::ActivityTimeIntegratedCommand()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityTimeIntegratedCommand::SetOptions(args_info_sydActivity & args_info)
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityTimeIntegratedCommand::Run(const Patient & patient,
                                             const RoiType & roitype,
                                             std::vector<std::string> & args)
{
  // Create an object to perform the fit
  syd::ActivityLambdaCommand * cmd = new syd::ActivityLambdaCommand;
  cmd->Initialize(adb_);

  // Initialize the values for the tac
  bool usePeak;
  syd::TimeActivityCurve tac;
  bool bb = cmd->GetTAC(patient, roitype, args, tac, usePeak);
  if (!bb) return;

  // Compute integration
  syd::TimeActivityCurveIntegrate a;
  a.SetInput(&tac);
  a.GetFitSolver().SetUseWeightedFit(!usePeak);
  a.Run();

  // Store output
  Activity activity;
  bool b = adb_->GetIfExist<Activity>(odb::query<Activity>::patient_id == patient.id and
                                      odb::query<Activity>::roi_type_id == roitype.id, activity);
  if (!b) activity = adb_->NewActivity(patient, roitype);

  // Debug
  /*
  for(auto i=0; i<tac.size(); i++)
    std::cout  << tac.GetTime(i) << " "
               << adb_->Get_CountByMM3_in_PercentInjectedActivityByKG(activity, tac.GetValue(i)) << std::endl;
  */

  // Update fields
  syd::TimeActivityCurveFitSolver & fit = a.GetFitSolver();
  activity.time_integrated_counts_by_mm3 = a.GetIntegratedValue();
  cmd->UpdateActivityFit(activity, fit);

  // Verbose FIXME VLOG(1)
  std::cout  << patient.synfrizz_id << " " << patient.name << " " << roitype.name << " "
             << adb_->Get_CountByMM3_in_MBqByKG(activity, activity.time_integrated_counts_by_mm3) << " "
             << adb_->Get_CountByMM3_in_PercentInjectedActivityByKG(activity, activity.fit_A)
             << " " << activity.fit_lambda << " " << activity.fit_error << " "
             << activity.fit_comment << " " << activity.fit_nb_points
             << " " << std::endl;
}
// --------------------------------------------------------------------
