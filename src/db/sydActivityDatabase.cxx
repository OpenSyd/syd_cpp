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
  if (args.size() < 3) {
    LOG(FATAL) << "Error need <cmd>, <patient> and <roiname>" << std::endl
               << "   <cmd> can be 'mean_count_by_mm3' or '%IA/kg' or '%OA/kg' or ''%PA/kg' or 'lambda'";
  }

  // cmd
  std::string cmd = args[0];
  args.erase(args.begin());

  if ((cmd != "mean_count_by_mm3") and (cmd != "%IA/kg")
      and (cmd != "%OA/kg") and (cmd != "%PA/kg")
      and (cmd != "lambda")) {
    LOG(FATAL) << "Error please provide 'mean_count_by_mm3' or '%IA/kg' or '%OA/kg' or '%PA/kg' or 'lambda' (you give '"
               << cmd << "')";
  }

  // patient
  std::vector<Patient> patients;
  std::string patientname = args[0];
  cdb_->GetPatientsByName(patientname, patients);
  args.erase(args.begin());

  // Loop on patient
  if (cmd == "lambda") DumpLambda(os, patients, args);
  else for(auto patient:patients) Dump(os, cmd, patient, args);
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
  double ia = syd::toDouble(patient.injected_quantity_in_MBq);
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
          ta << GetCountInPercentIAPerKG(activity, activity.mean_counts_by_mm3) //*d*k/ia*100;
             << GetCountInPercentIAPerKG(activity, activity.std_counts_by_mm3); //
        }

        if (cmd == "%OA/kg") {
          ta << activity.mean_counts_by_mm3*d*k/oa*100
             << activity.std_counts_by_mm3*d*k/oa*100;
        }

        if (cmd == "%PA/kg") {
          //ta << activity.peak_counts_by_mm3*d*k/ia*100;
          ta << GetCountInPercentIAPerKG(activity, activity.peak_counts_by_mm3);//*d*k/ia*100;
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
void syd::ActivityDatabase::DumpLambda(std::ostream & os, std::vector<Patient> & patients, std::vector<std::string> & args)
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

  // Loop on patients
  for(auto p:patients) {
    if (p.synfrizz_id != 0) {
      ta << p.synfrizz_id;
      // Loop over roi
      for(auto r:roitypes) {
        Activity a;
        bool b = GetIfExist<Activity>(odb::query<Activity>::patient_id == p.id and
                                      odb::query<Activity>::roi_type_id == r.id, a);
        bool fit_is_ok = false;
        if (b) {
          if (a.fit_lambda > 0.00001) {
            ta << log(2.0)/a.fit_lambda;
            fit_is_ok = true;
          }
          else ta << "-";
        }
        else ta << "-";
        if (fit_is_ok) {
          ta << GetCountInPercentIAPerKG(a, a.fit_A);//*d*k/ia*100;;
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
double syd::ActivityDatabase::GetCountInPercentIAPerKG(TimeActivity & timeactivity, double v)
{
   double k = (1.0/270199)*1000 * 1000; // 1000 is for g->kg and 1000 is for mm3 to cm3

   // Get patient
   Patient patient = cdb_->GetById<Patient>(timeactivity.patient_id);

   // Get first timepoint
   Timepoint timepoint = sdb_->GetById<Timepoint>(timeactivity.timepoint_id);
   // Get roimaskimage for this roitype
   RoiMaskImage roi = sdb_->GetById<RoiMaskImage>(timeactivity.roi_mask_image_id);

   // Compute
   double d = roi.density_in_g_cc;
   double ia = syd::toDouble(patient.injected_quantity_in_MBq);
   return v*d*k/ia*100;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::ActivityDatabase::GetCountInPercentIAPerKG(Activity & activity, double v)
{
   double k = (1.0/270199)*1000 * 1000; // 1000 is for g->kg and 1000 is for mm3 to cm3

   // Get patient
   Patient patient = cdb_->GetById<Patient>(activity.patient_id);

   // Get first timepoint
   Timepoint timepoint;
   bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::number == 1 and
                                        odb::query<Timepoint>::patient_id == patient.id, timepoint);
   if (!b) {
     // LOG(FATAL) << "Error no timepoint with number 1 for patient " << patient.name;
     return 0.0;
   }
   // Get roimaskimage for this roitype
   RoiMaskImage roi;
   b = sdb_->GetIfExist<RoiMaskImage>(odb::query<RoiMaskImage>::timepoint_id == timepoint.id and
                                      odb::query<RoiMaskImage>::roitype_id == activity.roi_type_id, roi);
   if (!b) {
     // LOG(FATAL) << "Error no roimaskimage for timepoint=" << timepoint.id << " and roitype=" << activity.roi_type_id;
     return 0.0;
   }

   // Compute
   double d = roi.density_in_g_cc;
   double ia = syd::toDouble(patient.injected_quantity_in_MBq);
   return v*d*k/ia*100;
}
// --------------------------------------------------------------------
