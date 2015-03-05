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
#include "sydTestFit_ggo.h"
#include "sydCommon.h"
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"
#include "sydImage.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

using namespace syd;

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydTestFit, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check arg
  if (args_info.inputs_num < 3) {
    cmdline_parser_sydTestFit_print_help();
    LOG(FATAL) << "Error, please provide at least 3 parameters.";
  }

  // Open database
  std::string db_name = args_info.inputs[0];
  std::string patient_name = args_info.inputs[1];
  std::string roi_name = args_info.inputs[2];
  std::shared_ptr<ActivityDatabase> adb = Database::OpenDatabaseType<ActivityDatabase>(db_name);
  std::shared_ptr<ClinicDatabase> cdb = adb->get_clinical_database();
  std::shared_ptr<StudyDatabase> sdb = adb->get_study_database();

  // Get patient, roi, data
  Patient patient = cdb->GetPatientByName(patient_name);
  RoiType roitype = cdb->GetRoiType(roi_name);
  std::vector<Timepoint> timepoints;
  sdb->GetTimepoints(patient, timepoints);
  int n = timepoints.size();
  std::vector<TimeActivity> timeactivities;
  RoiMaskImage roimaskimage;
  bool b = sdb->GetIfExist<RoiMaskImage>(odb::query<RoiMaskImage>::timepoint_id == timepoints[0].id and
                                         odb::query<RoiMaskImage>::roitype_id == roitype.id, roimaskimage);
  if (!b) {
    LOG(FATAL) << "Could not find mask " << roitype.name << " for timepoint " << timepoints[0];
  }

  for(auto t:timepoints) {
    TimeActivity ta;
    bool b = adb->GetIfExist<TimeActivity>(odb::query<TimeActivity>::timepoint_id == t.id and
                                           odb::query<TimeActivity>::patient_id == patient.id and // not needed
                                           odb::query<TimeActivity>::roi_mask_image_id == roimaskimage.id, ta);
    if (!b) {
      LOG(FATAL) << "Could not find timeactivity for tp " << t;
    }
    timeactivities.push_back(ta);
  }

  // Build tac
  syd::TimeActivityCurve tac;
  for(auto i=0; i<n; i++) {
    Timepoint t = timepoints[i];
    TimeActivity ta = timeactivities[i];
    double v = ta.mean_counts_by_mm3;
    //    double v = ta.peak_counts_by_mm3;
    v = v * 1000.0; // from mm3 to CC
    v = v / t.calibration_factor; // from count to MBq
    v = v * roimaskimage.density_in_g_cc * 1000.0; // by KG
    v = v / patient.injected_activity_in_MBq * 100; // in % IA
    tac.AddValue(t.time_from_injection_in_hours, v, 0.0);
  }
  DD(tac);

  // Fit
  syd::TimeActivityCurveFitSolver solver;
  solver.SetInput(&tac);

  solver.InitIncrementalRun();
  solver.IncrementalRun();
  DD(solver.GetFitA());
  DD(solver.GetFitLambda());
  double a = solver.GetFitA();
  double l =  solver.GetFitLambda();
  // std::cout << "plot 'a.txt' w l ; replot 'a.txt' w p ; l=0.010297405; "
  //   //            << "f(A,K1,K2,x)=A*exp(-l*x)*(exp(-K1*x)-exp((-K2*x))) ; replot f("
  //           << "g(A,lambda,x)=A*exp(-lambda*x); replot g("
  //           << solver.GetFitA() << ","
  //           << solver.GetFitLambda()
  //           << ",x); ";// << std::endl;

  solver.InitIncrementalRun();

  solver.Run_f4a();
  DD(solver.GetFitA());
  DD(solver.GetFitK1());
  DD(solver.GetFitK2());
  std::cout << "plot 'a.txt' w l ; replot 'a.txt' w p ; replot 'a.txt' using 1:3 w p; l=0.010297405; "
            << "g(A,lambda,x)=A*exp(-lambda*x); replot g("
            << a << "," << l
            << ",x); "
            << "f(A,K1,K2,x)=A*K1/(K2-K1)*exp(-l*x)*(exp(-K1*x)-exp((-K2*x))) ; replot f("
            << solver.GetFitA() << ","
            << solver.GetFitK1() << ","
            << solver.GetFitK2() << ",x);" << std::endl;


  // output
  double A = solver.GetFitA();
  double K1= solver.GetFitK1();
  double K2= solver.GetFitK2();
  std::ofstream os("a.txt");
  double error = 0.0;
  for(auto i=0; i<n; i++) {
    os << tac.GetTime(i) << " " << tac.GetValue(i) << " " << TwoExponential(A,K1,K2,tac.GetTime(i))
       << std::endl;
    error += pow((tac.GetValue(i) - TwoExponential(A,K1,K2,tac.GetTime(i))),2);
  }
  os.close();
  DD(error/2.0); // to check must be equal to ceres residual

  // This is the end, my friend.
}
// --------------------------------------------------------------------
