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

#include "sydQuery.h"
#include "syd_ROI_Time_Integrated_Activity_Command.h"
#include "syd_ROI_Peak_Command.h"
#include "syd_Dump_ROI_Activity_Command.h"
#include "sydOld_ggo.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydOld, args_info);

  // Start opening the db
  syd::sydQuery syd;
  syd.SetVerboseFlag(args_info.verbose_flag);
  syd.SetVerboseDBFlag(args_info.verboseDB_flag);
  syd.SetVerboseQueryFlag(args_info.verboseQuery_flag);
  syd.OpenDatabase();


  // =============================================================
  // INSERT
  // =============================================================


  // --------------------------------------------------------------
  if (args_info.insertRoi_given) {// FIXME
    if (args_info.inputs_num <4) FATAL("Requires 4 params : SynfrizzId StudyNb roiname mhd" << std::endl);
    Study study = syd.GetStudy(args_info.inputs);
    std::string roiname = args_info.inputs[2];
    std::string mhd = args_info.inputs[3];
    RoiStudy roistudy = syd.InsertRoi(study, roiname, mhd);
    syd.ComputeRoiInfo(roistudy);
  }

  // --------------------------------------------------------------
  if (args_info.insertSPECT_given) {// FIXME
    if (args_info.inputs_num <4) FATAL("Requires 4 params : SynfrizzId StudyNb dicom mhd" << std::endl);
    Study study = syd.GetStudy(args_info.inputs);
    std::string dcm = args_info.inputs[2];
    std::string mhd = args_info.inputs[3];
    Serie serie = syd.InsertSPECT(study, dcm, mhd);
    syd.ComputeTiming(study, serie);
  }

  // --------------------------------------------------------------
  if (args_info.insertCT_given) {// FIXME
    if (args_info.inputs_num <3) FATAL("Requires 3 params : SynfrizzId StudyNb mhd" << std::endl);
    Study study = syd.GetStudy(args_info.inputs);
    syd.InsertCT(study, args_info.inputs[2]);
  }


  // =============================================================
  // COMPUTE
  // =============================================================


  // --------------------------------------------------------------
  if (args_info.computeRoiTimeActivity_given) {// FIXME
    if (args_info.inputs_num <3) FATAL("Requires 3 params : SynfrizzId StudyNb roiname" << std::endl);
    Study study = syd.GetStudy(args_info.inputs);
    if (std::string(args_info.inputs[2]) == "all") {
      syd.ComputeRoiTimeActivity(study);
    }
    else {
      RoiStudy roistudy = syd.GetRoiStudy(study, args_info.inputs[2]);
      syd.ComputeRoiTimeActivity(roistudy);
    }
  }

  // --------------------------------------------------------------
  if (args_info.computeRoiInfo_given) {// FIXME
    if (args_info.inputs_num <3) FATAL("Requires 3 params : SynfrizzId StudyNb roiname" << std::endl);
    Study study = syd.GetStudy(args_info.inputs);
    syd.ComputeRoiInfo(study, args_info.inputs[2]);
  }

  // --------------------------------------------------------------
  if (args_info.computeRoiCumulActivity_given) {
    if (args_info.inputs_num != 3) FATAL("Requires 3 params : patient roi n=(nb points for fit)" << std::endl);
    std::vector<RoiStudy> roistudies;
    syd.GetRoiStudies(args_info.inputs[0], "all", args_info.inputs[1], roistudies);
    int n = atoi(args_info.inputs[2]);
    if ((n==0) || (n>4)) FATAL("Error with param n, must be in [2-4]");
    syd.ComputeRoiCumulActivity(roistudies, n);
  }

  // --------------------------------------------------------------
  if (args_info.computeRoiFitActivityTest_given) {
    if (args_info.inputs_num != 3) FATAL("Requires 3 params : patient roi n=(nb points for fit)" << std::endl);
    std::vector<RoiStudy> roistudies;
    syd.GetRoiStudies(args_info.inputs[0], "all", args_info.inputs[1], roistudies);
    int n = atoi(args_info.inputs[2]);
    if ((n<2) || (n>6)) FATAL("Error with param n, must be in [2-6]");
    syd.ComputeRoiFitActivityTest(roistudies, n);
  }

  // --------------------------------------------------------------
  if (args_info.computeRoiPeakCumulActivity_given) {
    if (args_info.inputs_num != 3) FATAL("Requires 3 params : patient roi g=(gauss)" << std::endl);
    std::vector<RoiStudy> roistudies;
    syd.GetRoiStudies(args_info.inputs[0], "all", args_info.inputs[1], roistudies);
    double g = atof(args_info.inputs[2]);
    syd.ComputeRoiPeakCumulActivity(roistudies, g);
  }

  // --------------------------------------------------------------
  if (args_info.computeCumulActivityImage_given) {
    if (args_info.inputs_num != 2) FATAL("Requires 2 params : patient n=(nb points for fit)" << std::endl);
    std::vector<Study> studies;
    syd.GetStudies(args_info.inputs[0], "all", studies);
    int n = atoi(args_info.inputs[1]);
    if ((n==0) || (n>4)) FATAL("Error with param n, must be in [2-4]");
    syd.ComputeCumulActivityImage(studies, n);
  }

  // --------------------------------------------------------------
  if (args_info.computeTiming_given) {// FIXME
    if (args_info.inputs_num <2) FATAL("Requires 2 params : SynfrizzId StudyNb" << std::endl);
    Study study = syd.GetStudy(args_info.inputs);
    syd.ComputeTiming(study);
  }


  // --------------------------------------------------------------
  if (args_info.Compute_ROI_Time_Integrated_Activity_given) {
    syd::ROI_Time_Integrated_Activity_Command x(syd);
    x.SetArgs(args_info.inputs, args_info.inputs_num);
    x.Run();
  }

  // --------------------------------------------------------------
  if (args_info.Compute_ROI_Peak_given) {
    syd::ROI_Peak_Command x(syd);
    x.SetArgs(args_info.inputs, args_info.inputs_num);
    x.Run();
  }

  // =============================================================
  // DUMP
  // =============================================================


  // --------------------------------------------------------------
  if (args_info.dumpStudy_given) {// FIXME
    if (args_info.inputs_num <1) FATAL("Requires 2 params : SynfrizzId [StudyNb=0]" << std::endl);
    unsigned long id = atoi(args_info.inputs[0]);
    unsigned long s = 0;
    if (args_info.inputs_num > 1) s = atoi(args_info.inputs[1]);
    Study study = syd.GetStudy(id, s);
    syd.DumpStudy(study);
  }

  // --------------------------------------------------------------
  if (args_info.dumpStudy2_given) {// FIXME
    if (args_info.inputs_num <1) FATAL("Requires 2 params : SynfrizzId [StudyNb=0]" << std::endl);
    unsigned long id = atoi(args_info.inputs[0]);
    unsigned long s = 0;
    if (args_info.inputs_num > 1) s = atoi(args_info.inputs[1]);
    Study study = syd.GetStudy(id, s);
    syd.DumpStudy2(study);
  }

  // --------------------------------------------------------------
  if (args_info.dumpCalibrationFactor_given) {// FIXME
    //if (args_info.inputs_num <1) FATAL("Requires 2 params : [SynfrizzId] [StudyNb=0]" << std::endl);
    if (args_info.inputs_num<1) syd.DumpCalibrationFactor();
    else {
      unsigned long id = atoi(args_info.inputs[0]);
      unsigned long s = 0;
      if (args_info.inputs_num > 1) s = atoi(args_info.inputs[1]);
      Study study = syd.GetStudy(id, s);
      syd.DumpCalibrationFactor(study);
    }
  }

  // --------------------------------------------------------------
  /*if (args_info.dumpActivity_given) {// FIXME
    if (args_info.inputs_num <3) FATAL("Requires 2+n params : organs/lesions/all type patient_numbers" << std::endl);
    std::string c = args_info.inputs[0];
    unsigned long s = 0;
    std::vector<int> ids;
    for(auto i=2; i<args_info.inputs_num; i++) ids.push_back(atoi(args_info.inputs[i]));
    syd.DumpActivity(c, ids, args_info.inputs[1]);
    }*/

  // --------------------------------------------------------------
  if (args_info.Dump_Activity_given) {
    syd::Dump_ROI_Activity_Command x(syd);
    x.SetArgs(args_info.inputs, args_info.inputs_num);
    x.Run();
  }

  // --------------------------------------------------------------
  if (args_info.dumpEffHalfLife_given) {// FIXME
    if (args_info.inputs_num <2) FATAL("Requires 2+n params : organs/lesions/all patient_numbers" << std::endl);
    std::string c = args_info.inputs[0];
    unsigned long s = 0;
    std::vector<int> ids;
    for(auto i=1; i<args_info.inputs_num; i++) ids.push_back(atoi(args_info.inputs[i]));
    syd.DumpEffectiveHalfLife(c, ids);
  }

  // --------------------------------------------------------------
  if (args_info.dump_given) {// FIXME
    if (args_info.inputs_num <3) FATAL("Requires 2+n params : organs/lesions/all type patient_numbers" << std::endl);
    std::string c = args_info.inputs[0];
    unsigned long s = 0;
    std::vector<int> ids;
    for(auto i=2; i<args_info.inputs_num; i++) ids.push_back(atoi(args_info.inputs[i]));
    syd.DumpPatientRoisValue(c, ids, args_info.inputs[1]);
  }
}
// --------------------------------------------------------------------
