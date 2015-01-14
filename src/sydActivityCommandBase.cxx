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
#include "sydActivityCommandBase.h"
#include "sydImage.h"

// --------------------------------------------------------------------
syd::ActivityCommandBase::ActivityCommandBase():DatabaseCommand()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommandBase::Initialize(std::shared_ptr<syd::ActivityDatabase> db,
                                              args_info_sydActivity & args_info)
{
  adb_ = db;
  cdb_ = adb_->get_clinical_database();
  sdb_ = adb_->get_study_database();
  SetOptions(args_info);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ActivityCommandBase::~ActivityCommandBase()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommandBase::Run(std::vector<std::string> args)
{
  // Loop on patients
  std::vector<Patient> patients;
  cdb_->GetPatientsByName(args[0], patients);
  args.erase(args.begin()); // (args is a copy)
  for(auto p:patients) Run(p, args);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommandBase::Run(const Patient & patient, std::vector<std::string> args)
{
  // Find the rois
  std::string roiname = args[0];
  args.erase(args.begin()); // (args is a copy)
  std::vector<RoiType> roitypes = sdb_->GetRoiTypes(roiname);
  if (roitypes.size() == 0) {
    LOG(FATAL) << "I found no roi named like : '" << roiname << "'";
  }

  // Loop
  for(auto r:roitypes) Run(patient, r, args);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommandBase::GetOrCreateTimeActivities(const Patient & patient,
                                                         const RoiType & roitype,
                                                         std::vector<Timepoint> & timepoints,
                                                         std::vector<TimeActivity> & timeactivities)
{
  // Get the timepoints
  if (timepoints.size() == 0)
    sdb_->LoadVector<Timepoint>(odb::query<Timepoint>::patient_id == patient.id, timepoints);

  // Get the roimaskimage (unique because first timepoints)
  RoiMaskImage roi;
  Timepoint reft;
  bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::number == 1 and
                                       odb::query<Timepoint>::patient_id == patient.id, reft);
  if (!b) {
    LOG(WARNING) << "Error not timepoint with number 1 for patient" << patient.name;
    return;
  }
  b = sdb_->GetIfExist<RoiMaskImage>(odb::query<RoiMaskImage>::timepoint_id == reft.id and
                                          odb::query<RoiMaskImage>::roitype_id == roitype.id,
                                          roi);
  if (!b) return;

  // Get/build list of timeactivity
  timeactivities.clear();
  for (auto tp:timepoints) {
    TimeActivity timeactivity;
    bool b = adb_->GetIfExist<TimeActivity>(odb::query<TimeActivity>::timepoint_id == tp.id and
                                            odb::query<TimeActivity>::roi_mask_image_id == roi.id and
                                            odb::query<TimeActivity>::patient_id == patient.id,
                                            timeactivity);
    if (!b) {
      Timepoint timepoint = sdb_->GetById<Timepoint>(roi.timepoint_id);
      VLOG(2) << "Creating new timeactivity for " << patient.name << " tp="
              << timepoint.number << " roi= " <<  roitype.name;
      timeactivity = adb_->NewTimeActivity(timepoint, roi);
    }
    timeactivities.push_back(timeactivity);
  }
}
// --------------------------------------------------------------------
