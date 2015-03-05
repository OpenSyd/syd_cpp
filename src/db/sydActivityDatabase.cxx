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
#include "sydActivityDatabase.h"

// --------------------------------------------------------------------
syd::ActivityDatabase::ActivityDatabase(std::string name, std::string param):Database(name)
{
  // List all params
  std::istringstream f(param);
  SetFileAndFolder(f);
  std::string cdb_name;
  if (!getline(f, cdb_name, ';')) {
    LOG(FATAL) << "Error while parsing cdb name for folder. db is "
               << name << " (" << get_typename() << ") params = " << param;
  }
  std::string sdb_name;
  if (!getline(f, sdb_name, ';')) {
    LOG(FATAL) << "Error while parsing sdb name for folder. db is "
               << name << " (" << get_typename() << ") params = " << param;
  }

  OpenSqliteDatabase(filename_, folder_);

  cdb_ = syd::Database::OpenDatabaseType<ClinicDatabase>(cdb_name);
  sdb_ = syd::Database::OpenDatabaseType<StudyDatabase>(sdb_name);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ActivityDatabase::~ActivityDatabase()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::CreateDatabase()
{
  DD("CreateDatabase");

  // DD("here");
  // odb::transaction t (db_->begin());
  // odb::schema_catalog::create_schema (*db_);
  // t.commit();

  DD("TODO");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::CheckIntegrity(std::vector<std::string> & args)
{
  DD("TODO ActivityDatabase::CheckIntegrity");
  DDS(args);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Activity syd::ActivityDatabase::NewActivity(const Patient & patient, const RoiType & roitype)
{
  Activity activity;
  activity.patient_id = patient.id;
  activity.roi_type_id = roitype.id;
  Insert(activity);
  return activity;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeActivity syd::ActivityDatabase::NewTimeActivity(const Timepoint & timepoint,
                                                         const RoiMaskImage & roi)
{
  Patient patient(sdb_->GetPatient(timepoint));
  TimeActivity ta;
  ta.timepoint_id = timepoint.id;
  ta.patient_id = patient.id;
  ta.roi_mask_image_id = roi.id;
  Insert(ta);
  return ta;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::Dump(std::ostream & os, std::vector<std::string> & args)
{
  std::vector<std::string> cmds;
  cmds.push_back("mean_count_by_mm3");
  cmds.push_back("%IA/kg");
  cmds.push_back("%OA/kg");
  cmds.push_back("%PA/kg");
  cmds.push_back("lambda");
  cmds.push_back("tia");
  cmds.push_back("MBq.h");
  cmds.push_back("w"); // weight correlation
  cmds.push_back("dose");
  std::string allcmds;
  for(auto i:cmds) allcmds+=i+" ";

  if (args.size() < 3) {
    LOG(FATAL) << "Error need <cmd>, <patient> and <roiname>" << std::endl
               << "   <cmd> can be " << allcmds;
  }

  // cmd
  std::string cmd = args[0];
  args.erase(args.begin());
  bool found=false;
  for(auto i:cmds) found = found or (i==cmd);

  if (!found) {
    LOG(FATAL) << "Error ! Please set cmd among : " << allcmds << std::endl
               << "(you provide '" << cmd << "')";
  }

  // patient
  std::vector<Patient> patients;
  std::string patientname = args[0];
  cdb_->GetPatientsByName(patientname, patients);
  args.erase(args.begin());

  // Loop on patient
  if (cmd == "lambda") { DumpLambda(os, patients, args); return; }
  if (cmd == "tia") { DumpTimeIntegratedActivities(os, patients, args, "%ID/kg"); return; }
  if (cmd == "MBq.h") { DumpTimeIntegratedActivities(os, patients, args, "MBq.h"); return; }
  if (cmd == "w") { DumpWeight(os, patients, args); return; }
  if (cmd == "dose") { DumpDose(os, patients, args); return; }
  for(auto patient:patients) Dump(os, cmd, patient, args);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::Dump(std::ostream & os, const std::string & cmd,
                                 const Patient & patient, std::vector<std::string> & args)
{
  // Get first timepoint
  Timepoint timepoint;
  bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::number == 1 and
                                       odb::query<Timepoint>::patient_id == patient.id, timepoint);
  if (!b) {
    return; //LOG(FATAL) << "Error no timepoint with number 1 for patient " << patient.name;
  }

  // Get roi
  std::string roiname = args[0];
  std::vector<RoiMaskImage> roimaskimages =
    sdb_->GetRoiMaskImages(timepoint, roiname);
  if (roimaskimages.size() == 0) {
    LOG(WARNING) << "No roimaskimage found with name '" << roiname << "'";
    return; //LOG(FATAL) << "No roimaskimage found with name '" << roiname << "'";
  }
  // Sort by id
  std::sort(begin(roimaskimages), end(roimaskimages),
            [&roimaskimages](RoiMaskImage a, RoiMaskImage b) { return a.roitype_id < b.roitype_id; }  );


  // Prepare to print
  syd::PrintTable ta;
  ta.AddColumn("Nb", 3, 0);
  ta.AddColumn("t", 9, 2);
  ta.AddColumn("%", 7, 2);

  // loop on roi to get all timeactivities
  int nb = 0;
  std::vector<std::vector<TimeActivity>> tva;
  for(auto roi:roimaskimages) {
    // Retrieve all timepoints and sort
    std::vector<TimeActivity> timeactivities;
    LoadVector<TimeActivity>(odb::query<TimeActivity>::patient_id == patient.id and
                             odb::query<TimeActivity>::roi_mask_image_id == roi.id, timeactivities);
    std::sort(begin(timeactivities), end(timeactivities),
              [this](TimeActivity a, TimeActivity b) {
                Timepoint ta(sdb_->GetById<Timepoint>(a.timepoint_id));
                Timepoint tb(sdb_->GetById<Timepoint>(b.timepoint_id));
                return ta.number < tb.number; }  );
    if (timeactivities.size() > nb) nb = timeactivities.size();
    tva.push_back(timeactivities);
    RoiType roitype(sdb_->GetRoiType(roi));

    if (cmd == "mean_count_by_mm3" ) {
      ta.AddColumn(roitype.name, 11, 3);
      ta.AddColumn("std", 7, 2);
    }
    if (cmd == "%IA/kg") {
      ta.AddColumn(roitype.name, 16, 8);
      ta.AddColumn("std", 16, 8);
    }
    if (cmd == "%OA/kg") {
      ta.AddColumn(roitype.name, 16, 8);
      ta.AddColumn("std", 16, 8);
    }
    if (cmd == "%PA/kg") {
      ta.AddColumn(roitype.name, 16, 8);
      //ta.AddColumn("std", 16, 8);
    }
  }

  // get the roimask for 'patient' of the first timepoint
  Timepoint t(sdb_->GetById<Timepoint>(tva[0][0].timepoint_id));
  RoiMaskImage patientroi = sdb_->GetRoiMaskImage(t, "patient");
  double td = patientroi.density_in_g_cc;

  // get mean activity
  double k = (1.0/270199)*1000 * 1000; // 1000 is for g->kg and 1000 is for mm3 to cm3
  double ia = patient.injected_activity_in_MBq;
  ta.Init();

  for(auto i=0; i<nb; i++) {
    Timepoint t(sdb_->GetById<Timepoint>(tva[0][i].timepoint_id));
    ta << t.number
       << t.time_from_injection_in_hours;
    if (cmd == "%OA/kg")
      ta << 100/(patientroi.volume_in_cc*td/1000); // threshold
    else
      ta << 100/(patient.weight_in_kg); // threshold

    TimeActivity total_activity;
    GetIfExist<TimeActivity>(odb::query<TimeActivity>::patient_id == patient.id and
                             odb::query<TimeActivity>::roi_mask_image_id == patientroi.id and
                             odb::query<TimeActivity>::timepoint_id == t.id,
                             total_activity);
    double oa = total_activity.mean_counts_by_mm3*td*patientroi.volume_in_cc*td*k/1000.0;

    for(auto j=0; j<roimaskimages.size(); j++) {
      RoiMaskImage roi(roimaskimages[j]);
      double d = roi.density_in_g_cc;
      if (i<tva[j].size()) {
        TimeActivity activity(tva[j][i]);

        if (cmd == "mean_count_by_mm3" ) {
          ta << activity.mean_counts_by_mm3
             << activity.std_counts_by_mm3;
        }

        if (cmd == "%IA/kg") {
          //          ta << activity.mean_counts_by_mm3*d*k/ia*100
          //   << activity.std_counts_by_mm3*d*k/ia*100;
          ta << Get_CountByMM3_in_PercentInjectedActivityByKG(activity, activity.mean_counts_by_mm3) //*d*k/ia*100;
             << Get_CountByMM3_in_PercentInjectedActivityByKG(activity, activity.std_counts_by_mm3); //
        }

        if (cmd == "%OA/kg") {
          ta << activity.mean_counts_by_mm3*d*k/oa*100
             << activity.std_counts_by_mm3*d*k/oa*100;
        }

        if (cmd == "%PA/kg") {
          //ta << activity.peak_counts_by_mm3*d*k/ia*100;
          ta << Get_CountByMM3_in_PercentInjectedActivityByKG(activity, activity.peak_counts_by_mm3);//*d*k/ia*100;
          //  << activity.std_counts_by_mm3*d*k/oa*100;
        }

      }
      else {
        ta << "-" << "-";
      }
    }
  }

  // Final indication of (first) roi size
  RoiMaskImage roi(roimaskimages[0]);
  RoiType roitype(sdb_->GetRoiType(roi));
  std::cout << "# " << patient.name << " " << patient.synfrizz_id << " "
            << roitype.name << " " << roi.volume_in_cc << " cc "
            << roi.density_in_g_cc*roi.volume_in_cc << " g"
            << "    threshold is " << 100/(patient.weight_in_kg) << "%"
            << std::endl;
  ta.Print(std::cout);
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::DumpLambda(std::ostream & os,
                                       std::vector<Patient> & patients,
                                       std::vector<std::string> & args)
{

  // Get list of roitypes
  std::string roiname = args[0];
  std::vector<RoiType> roitypes = sdb_->GetRoiTypes(roiname);

  // Prepare to print
  syd::PrintTable ta;
  ta.AddColumn("#P", 3, 0);
  for(auto r:roitypes) {
    ta.AddColumn(r.name, 12, 1); // lambda
    ta.AddColumn("A", 12, 2); // A
    ta.AddColumn("err", 12, 4); // error
    ta.AddColumn("nb", 5, 0); // nb_point
  }

  double ratio_threshold = 0.0; //FIXME

  // Loop on patients
  for(auto p:patients) {
    if (p.synfrizz_id != 0) {
      ta << p.synfrizz_id;

      // Get roi=liver for this patient
      RoiType liverroi = cdb_->GetRoiType("liver");
      Activity liver;
      GetIfExist<Activity>(odb::query<Activity>::patient_id == p.id and
                           odb::query<Activity>::roi_type_id == liverroi.id, liver);
      double liver_activity = Get_CountByMM3_in_MBqByKG(liver, liver.time_integrated_counts_by_mm3);

      // Loop over roi
      for(auto r:roitypes) {
        Activity a;
        bool b = GetIfExist<Activity>(odb::query<Activity>::patient_id == p.id and
                                      odb::query<Activity>::roi_type_id == r.id, a);
        bool fit_is_ok = false;
        if (b) {

          // Conditional display of lambda, only if ratio with liver greater than a threshold
          double ac = Get_CountByMM3_in_MBqByKG(a, a.time_integrated_counts_by_mm3);
          if (ac/liver_activity > ratio_threshold) {
            //if (a.fit_lambda > 0.00001) {
            ta << log(2.0)/a.fit_lambda;
            fit_is_ok = true;
          }
          else ta << "-";
        }
        else ta << "-";
        if (fit_is_ok) {
          ta << Get_CountByMM3_in_PercentInjectedActivityByKG(a, a.fit_A);//*d*k/ia*100;;
          ta << a.fit_error;
          ta << a.fit_nb_points;
        } else ta << "-" << "-" << "-";

      }
    }
  }
  ta.Print(std::cout);
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::DumpTimeIntegratedActivities(std::ostream & os,
                                                         std::vector<Patient> & patients,
                                                         std::vector<std::string> & args,
                                                         std::string unit)
{
  // Get list of roitypes
  std::string roiname = args[0];
  std::vector<RoiType> roitypes = sdb_->GetRoiTypes(roiname);

  // Prepare to print
  syd::PrintTable ta;
  ta.AddColumn("#P", 3, 0);
  for(auto r:roitypes) {
    ta.AddColumn(r.name, 12, 1); // time_integrated_counts_by_mm3
    ta.AddColumn("ratio", 12, 1); // ratio over liver
  }

  // Loop on patients
  for(auto p:patients) {
    if (p.synfrizz_id != 0) {
      ta << p.synfrizz_id;

      // Get roi=liver for this patient
      RoiType liverroi = cdb_->GetRoiType("liver");
      Activity liver;
      GetIfExist<Activity>(odb::query<Activity>::patient_id == p.id and
                           odb::query<Activity>::roi_type_id == liverroi.id, liver);
      // Loop over roi
      for(auto r:roitypes) {
        Activity a;
        bool b = GetIfExist<Activity>(odb::query<Activity>::patient_id == p.id and
                                      odb::query<Activity>::roi_type_id == r.id, a);
        if (b) {
          if (a.time_integrated_counts_by_mm3 > 0.000001) {
            //            ta << Get_CountByMM3_in_PercentInjectedActivityByKG(a, a.time_integrated_counts_by_mm3);
            //            double ac = Get_CountByMM3_in_MBqByKG(a, a.time_integrated_counts_by_mm3);

            if (unit == "MBq.h") {
              double ac = a.time_integrated_counts_by_mm3;
              ac = Get_CountByMM3_in_MBqByCC(ac);
              ta << ac;
              ta << ac/(Get_CountByMM3_in_MBqByCC(liver.time_integrated_counts_by_mm3));
            }
            else {
              double ac = Get_CountByMM3_in_PercentInjectedActivityByKG(a, a.time_integrated_counts_by_mm3)/100;
              ta << ac;
              ta << ac/(Get_CountByMM3_in_PercentInjectedActivityByKG(liver, liver.time_integrated_counts_by_mm3)/100);
            }
          }
          else { ta << "-"; ta << "-"; }
        }
        else { ta << "-"; ta << "-"; }
      }
    }
  }
  ta.Print(std::cout);
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::DumpDose(std::ostream & os,
                                     std::vector<Patient> & patients,
                                     std::vector<std::string> & args)
{
  // Get list of roitypes
  std::string roiname = args[0];
  std::vector<RoiType> roitypes = sdb_->GetRoiTypes(roiname);

  // Prepare to print
  syd::PrintTable ta;
  ta.AddColumn("#P", 3, 0);
  for(auto r:roitypes) {
    ta.AddColumn(r.name, 12, 5); // dose by injected MBq, in cGy
    ta.AddColumn("ratio", 12, 4); // ratio over liver
    // ta.AddColumn("uncert%", 12, 1); // % MC uncertainty
  }

  // Loop on patients
  for(auto p:patients) {
    if (p.synfrizz_id != 0) {
      ta << p.synfrizz_id;

      // Get roi=liver for this patient
      RoiType liverroi = cdb_->GetRoiType("liver");
      Activity liver;
      GetIfExist<Activity>(odb::query<Activity>::patient_id == p.id and
                           odb::query<Activity>::roi_type_id == liverroi.id, liver);
      // Loop over roi
      for(auto r:roitypes) {
        Activity a;
        bool b = GetIfExist<Activity>(odb::query<Activity>::patient_id == p.id and
                                      odb::query<Activity>::roi_type_id == r.id, a);
        if (b) {
          ta << a.mean_dose*100
             << a.mean_dose/liver.mean_dose;
             // << a.mean_uncertainty*100;
          }
        else { ta << "-" << "-"; } // << "-"; }
      }
    }
  }
  ta.Print(std::cout);
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::DumpWeight(std::ostream & os,
                                       std::vector<Patient> & patients,
                                       std::vector<std::string> & args)
{
  // Get list of roitypes
  RoiType liverroi(cdb_->GetRoiType("liver"));
  //  DD(liverroi);

  // Prepare to print
  syd::PrintTable ta;
  ta.AddColumn("#P", 3, 0);
  ta.AddColumn("w", 10, 1); // weight_in_kg
  ta.AddColumn("max", 10, 3); // max liver activity
  ta.AddColumn("ti", 10, 4); // time integrated liver activity

  // Loop on patients
  //  DDS(patients);
  for(auto patient:patients) {
    if (patient.synfrizz_id != 0) {

      // Get the first timepoint
      Timepoint timepoint;
      bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::number == 1 and
                                           odb::query<Timepoint>::patient_id == patient.id,
                                           timepoint);
      if (!b) continue;

      ta << patient.synfrizz_id;
      ta << patient.weight_in_kg;

      // Get the liver mask for this patient
      RoiMaskImage liver = sdb_->GetRoiMaskImage(timepoint, "liver");
      //      DD(liver);

      // Get the list of activities
      std::vector<TimeActivity> tactivities;
      LoadVector<TimeActivity>(odb::query<TimeActivity>::patient_id == patient.id and
                               odb::query<TimeActivity>::roi_mask_image_id == liver.id,
                               tactivities);
      //      DDS(tactivities);

      // Get the max
      double max = 0.0;
      TimeActivity ac;
      for(auto a:tactivities) {
        if (a.mean_counts_by_mm3 > max) {
          max = a.mean_counts_by_mm3;
          ac = a;
        }
      }
      ta << Get_CountByMM3_in_PercentInjectedActivityByKG(ac, max);

      // Get activity
      Activity activity;
      GetIfExist<Activity>(odb::query<Activity>::patient_id == patient.id and
                           odb::query<Activity>::roi_type_id == liverroi.id, activity);
      ta << Get_CountByMM3_in_PercentInjectedActivityByKG(activity, activity.time_integrated_counts_by_mm3)/100;

    }
  }
  ta.Print(std::cout);
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::ActivityDatabase::Get_CountByMM3_in_PercentInjectedActivityByKG(TimeActivity & timeactivity, double v)
{
  v = Get_CountByMM3_in_MBqByKG(timeactivity, v);
  Patient patient = cdb_->GetById<Patient>(timeactivity.patient_id);
  double ia = patient.injected_activity_in_MBq;
  return v/ia*100;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::ActivityDatabase::Get_CountByMM3_in_PercentInjectedActivityByKG(Activity & activity, double v)
{
  v = Get_CountByMM3_in_MBqByKG(activity, v);
  Patient patient = cdb_->GetById<Patient>(activity.patient_id);
  double ia = patient.injected_activity_in_MBq;
  return v/ia*100;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::ActivityDatabase::Get_CountByMM3_in_MBqByKG(Activity & activity, double v)
{
  // Convert in MBq by cc
  v = Get_CountByMM3_in_MBqByCC(v);

  // Get the density
  RoiMaskImage roimask;
  bool b = GetRoiMaskImage(activity, roimask);
  if (!b) return 0.0;

  // Compute the final value
  double d = roimask.density_in_g_cc;
  return v*d*1000;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::ActivityDatabase::Get_CountByMM3_in_MBqByKG(TimeActivity & timeactivity, double v)
{
  // Convert in MBq by cc
  v = Get_CountByMM3_in_MBqByCC(v);

  // Get the density
  RoiMaskImage roimask = sdb_->GetById<RoiMaskImage>(timeactivity.roi_mask_image_id);

  // Compute the final value
  double d = roimask.density_in_g_cc;
  return v*d*1000;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::ActivityDatabase::GetRoiMaskImage(Activity & activity, RoiMaskImage & roimask)
{
  // Get patient
  Patient patient = cdb_->GetById<Patient>(activity.patient_id);

  // Get first timepoint
  Timepoint timepoint;
  bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::number == 1 and
                                       odb::query<Timepoint>::patient_id == patient.id, timepoint);
  if (!b) return false;

  // Get roimaskimage for this roitype
  b = sdb_->GetIfExist<RoiMaskImage>(odb::query<RoiMaskImage>::timepoint_id == timepoint.id and
                                     odb::query<RoiMaskImage>::roitype_id == activity.roi_type_id, roimask);
  return b;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::ActivityDatabase::Get_CountByMM3_in_MBqByCC(double v)
{
  // Calibration factor is hard-coded : to be changed !
  LOG(FATAL) << "OLD CALIBRATION !! TO CHANGE";
  double k = (1.0/270199)*1000.0; // 1000 is for mm3 to cm3
  return v*k;
}
// --------------------------------------------------------------------
