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
#include "sydActivityLambdaCommand.h"
#include "sydImage.h"

// --------------------------------------------------------------------
syd::ActivityLambdaCommand::ActivityLambdaCommand()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityLambdaCommand::SetOptions(args_info_sydActivity & args_info)
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityLambdaCommand::Run(const Patient & patient,
                                     const RoiType & roitype,
                                     std::vector<std::string> & args)
{
  bool usePeak;
  syd::TimeActivityCurve tac;
  GetTAC(patient, roitype, args, tac, usePeak);
  DD(usePeak);
  DD(tac.size());

  // Compute
  syd::TimeActivityCurveFitSolver a;
  a.SetInput(&tac);
  //  --> options, std, monoexpo, nb of samples etc
  a.SetUseWeightedFit(!usePeak);
  a.Run();

  // Store output
  Activity activity;
  bool b = adb_->GetIfExist<Activity>(odb::query<Activity>::patient_id == patient.id and
                                      odb::query<Activity>::roi_type_id == roitype.id, activity);
  if (!b) activity = adb_->NewActivity(patient, roitype);

  // Update the activity fields
  UpdateActivityFit(activity, a);

  // DEBUG
  /*
    RoiMaskImage roi = sdb_->GetById<RoiMaskImage>(timeactivities[0].roi_mask_image_id);
    double d = roi.density_in_g_cc;
    double k = (1.0/270199)*1000 * 1000; // FIXME to be changed
    double ia = syd::toDouble(patient.injected_quantity_in_MBq);
    double id = a.GetA()*d*k/ia*100;
    std::cout << "replot  f(" << id << " , " << activity.fit_lambda << ",x);" << std::endl;
    double half_life = log(2.0)/activity.fit_lambda;
    double indium = log(2.0)/0.01029583;
  */

  // Verbose FIXME VLOG(1)
  std::cout  << patient.synfrizz_id << " " << patient.name << " " << roitype.name << " "
             << adb_->GetCountInPercentIAPerKG(activity, activity.fit_A)
             << " " << activity.fit_lambda << " " << activity.fit_error << " "
             << activity.fit_comment << " " << activity.fit_nb_points << std::endl;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityLambdaCommand::GetTAC(const Patient & patient,
                                        const RoiType & roitype,
                                        std::vector<std::string> & args,
                                        syd::TimeActivityCurve & tac,
                                        bool & usePeak)
{
  usePeak = false;
  if (args.size() != 0) {
    if (args[0] == "peak") {
      usePeak = true;
      args.erase(args.begin()); // (args is a copy)
    }
    if (args.size() != 0) {
      std::string a;
      for (auto s:args) a += " "+s;
      LOG(WARNING) << "Warning, ignoring the options : " << a;
    }
  }

  // Get or create the list of timeactivity
  std::vector<TimeActivity> timeactivities;
  std::vector<Timepoint> fake;
  GetOrCreateTimeActivities(patient, roitype, fake, timeactivities);
  if (timeactivities.size() == 0) return;

  // Create the TAC
  for (auto ta:timeactivities) {
    double m,std;
    if (usePeak) {
      m = ta.peak_counts_by_mm3;
      std = 0.0;
    }
    else {
      m = ta.mean_counts_by_mm3;
      std = ta.std_counts_by_mm3;
    }
    Timepoint tp = sdb_->GetById<Timepoint>(ta.timepoint_id);
    double t = tp.time_from_injection_in_hours;
    tac.AddValue(t,m,std*std);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityLambdaCommand::UpdateActivityFit(Activity & activity,
                                                   syd::TimeActivityCurveFitSolver & a)
{
  activity.fit_lambda = a.GetLambda();
  activity.fit_A = a.GetFitA();
  activity.fit_error = a.GetFitError();
  activity.fit_nb_points = a.GetFitNbPoints();
  activity.fit_comment = a.GetFitComment();
  adb_->Update(activity);
}
// --------------------------------------------------------------------
