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
#include "sydActivityCommand.h"
#include "sydImage.h"

// --------------------------------------------------------------------
syd::ActivityCommand::ActivityCommand(std::string db):DatabaseCommand()
{
  adb_ = syd::Database::OpenDatabaseType<ActivityDatabase>(db);
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ActivityCommand::ActivityCommand(syd::ActivityDatabase  * db):
  DatabaseCommand(), adb_(db)
{
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommand::Initialization()
{
  cdb_ = adb_->get_clinical_database();
  sdb_ = adb_->get_study_database();
  set_mean_radius(5);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ActivityCommand::~ActivityCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommand::Run(std::vector<std::string> & args)
{
  if (args.size() == 0) {
    LOG(FATAL) << "Error, please provide a <cmd>. See --help";
  }

  // set the radius for peak computation
  adb_->set_mean_radius(mean_radius_);

  // Switch according to command
  std::string cmd = args[0];
  args.erase(args.begin());

  if (cmd == "ta") { peakActivityFlag_ = false; RunTimeActivity(args); return; } // ta = timeactivity
  if (cmd == "pa") { peakActivityFlag_ = true; RunTimeActivity(args); return; } // peak activity
  if (cmd == "ia") { RunIntegratedActivity(args);  return; } // integrated activity

  LOG(FATAL) << "Error, please provide 'ta' or 'ia' or 'pa'. See --help";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommand::RunTimeActivity(std::vector<std::string> args)
{
  if (args.size() == 0) {
    LOG(FATAL) << "Error, please provide a <patientname>";
  }
  std::vector<Patient> patients;
  cdb_->GetPatientsByName(args[0], patients);
  args.erase(args.begin()); // (args is a copy)
  for(auto p:patients) {
    RunTimeActivity(p, args);
  }
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
void syd::ActivityCommand::RunTimeActivity(const Patient & patient, std::vector<std::string> args)
{
  std::vector<Timepoint> timepoints;
  if (args.size() < 2) {
    LOG(FATAL) << "Error, need timepoint number and roi name";
  }
  std::string s = args[0];
  if (s == "all") {
    sdb_->LoadVector<Timepoint>(odb::query<Timepoint>::patient_id == patient.id, timepoints);
  }
  else {
    int n = atoi(s.c_str());
    Timepoint t;
    bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::number == n and
                                         odb::query<Timepoint>::patient_id == patient.id, t);
    if (!b) {
      LOG(FATAL) << "Error I could not find timepoint number " << n << " for the patient " << patient.name;
    }
    timepoints.push_back(t);
  }

  // Remove first args (args is a copy)
  args.erase(args.begin());

  // Loop over timepoints
  for(auto t:timepoints) RunTimeActivity(t, args);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommand::RunTimeActivity(const Timepoint & timepoint, std::vector<std::string> args)
{
  // Get patient
  Patient patient(sdb_->GetPatient(timepoint));

  // Retrieve reference timepoint (needed to load the mask at this ref timepoint)
  Timepoint reft;
  bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::number == 1 and
                                       odb::query<Timepoint>::patient_id == patient.id, reft);
  if (!b) {
    LOG(FATAL) << "Error not timepoint with number 1 for patient" << patient.name;
  }

  // find all roitypes
  std::string roiname = args[0];
  std::vector<RoiType> roitypes;
  if (roiname == "all") cdb_->LoadVector<RoiType>(roitypes);
  else cdb_->LoadVector<RoiType>(odb::query<RoiType>::name.like(roiname), roitypes);
  if (roitypes.size() == 0) {
    LOG(WARNING) << "I found no roi named : '" << roiname << "'";
  }

  // Get corresponding roimaskimage
  std::vector<RoiMaskImage> rois;
  for(auto rt:roitypes) {
    RoiMaskImage roi;
    bool b = sdb_->GetIfExist<RoiMaskImage>(odb::query<RoiMaskImage>::timepoint_id == reft.id and
                                            odb::query<RoiMaskImage>::roitype_id == rt.id, roi);
    if (b) rois.push_back(roi);
  }

  // Loop on rois
  for(auto r:rois) {
    TimeActivity timeactivity;
    RoiType roitype = cdb_->GetById<RoiType>(r.roitype_id);
    bool b = adb_->GetIfExist<TimeActivity>(odb::query<TimeActivity>::timepoint_id == timepoint.id and
                                            odb::query<TimeActivity>::roi_id == r.id,
                                            timeactivity);
    if (!b) {
      VLOG(2) << "Creating new timeactivity for " << patient.name << " tp="
              << timepoint.number << " roi= " <<  roitype.name;
      timeactivity = adb_->NewTimeActivity(timepoint, r);
    }
    // not required FIXME
    timeactivity.patient_id = patient.id;
    timeactivity.roi_id = r.id;
    timeactivity.timepoint_id = timepoint.id;

    // Compute and update activity
    if (peakActivityFlag_ == false) adb_->UpdateTimeActivityInRoi(timeactivity);
    else adb_->UpdatePeakTimeActivityInRoi(timeactivity);

    // Verbose if needed
    VLOG(1) << patient.synfrizz_id << " " << patient.name << " " << roitype.name << " "
            << timepoint.number  << " " << timepoint.time_from_injection_in_hours  << " "
            << timeactivity.mean_counts_by_mm3 << " " << timeactivity.std_counts_by_mm3 << " "
            << timeactivity.peak_counts_by_mm3 << " " << timeactivity.peak_position;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommand::RunIntegratedActivity(std::vector<std::string> & args)
{
  if (args.size() == 0) {
    LOG(FATAL) << "Error, please provide a <patientname>";
  }
  std::vector<Patient> patients;
  cdb_->GetPatientsByName(args[0], patients);
  args.erase(args.begin());
  for(auto p:patients) {
    RunIntegratedActivity(p, args);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommand::RunIntegratedActivity(const Patient & patient, std::vector<std::string> & args)
{

}
// --------------------------------------------------------------------
