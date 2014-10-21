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

// itk
#include <itkLabelStatisticsImageFilter.h>

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
  std::vector<Patient> patients;
  cdb_->GetPatientsByName(args[0], patients);
  args.erase(args.begin());
  for(auto p:patients) {
    Run(p, args);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommand::Run(const Patient & patient, std::vector<std::string> & args)
{
  std::vector<Timepoint> timepoints;
  if (args.size() < 2) {
    LOG(FATAL) << "Error, need timepoint number and roi name";
  }
  std::string s = args[0];
  if (s == "all") {
    sdb_->LoadVector<Timepoint>(timepoints, odb::query<Timepoint>::patient_id == patient.id);
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

  // Remove first args
  args.erase(args.begin());

  // Loop over timepoints
  for(auto t:timepoints) Run(t, args);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommand::Run(const Timepoint & timepoint, std::vector<std::string> & args)
{
  // Get patient
  Patient patient(sdb_->GetPatient(timepoint));

  // Retrieve reference timepoint
  Timepoint reft;
  bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::number == 1 and
                                       odb::query<Timepoint>::patient_id == patient.id, reft);
  if (!b) {
    LOG(FATAL) << "Error not timepoint with number 1 for patient" << patient.name;
  }

  // find all roitypes
  std::string roiname = args[0];
  std::vector<RoiType> roitypes;
  cdb_->LoadVector<RoiType>(roitypes, odb::query<RoiType>::name.like(roiname));

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
      VLOG(1) << "Creating new timeactivity for " << patient.name << " tp="
              << timepoint.number << " roi= " <<  roitype.name;
      timeactivity = adb_->NewTimeActivity(timepoint, r);
    }
    // not required FIXME
    timeactivity.patient_id = patient.id;
    timeactivity.roi_id = r.id;
    timeactivity.timepoint_id = timepoint.id;
    UpdateActivityInRoi(timepoint, r, timeactivity);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommand::UpdateActivityInRoi(const Timepoint & timepoint,
                                               const RoiMaskImage & roi,
                                               TimeActivity & timeactivity)
{
  // Load spect image
  RawImage ispect = sdb_->GetById<RawImage>(timepoint.spect_image_id);
  std::string fspect = sdb_->GetImagePath(ispect);
  ImageType::Pointer spect = syd::ReadImage<ImageType>(fspect);

  // Load roi mask
  std::string fmask = sdb_->GetImagePath(roi);
  MaskImageType::Pointer mask = syd::ReadImage<MaskImageType>(fmask);

  // in general spect and mask not the same spacing. Need to resample
  // one of the two.

  // resample mask like the spect image (both spacing and crop).
  // FIXME or reverse ... --> but need to store correct volume for
  // the counts if resample, need to convert values in
  // counts_concentration
  mask = syd::ResampleImageLike<MaskImageType>(mask, spect, 0, 0);

  // compute stats
  typedef itk::LabelStatisticsImageFilter<ImageType, MaskImageType> FilterType;
  typename FilterType::Pointer filter=FilterType::New();
  filter->SetInput(spect);
  filter->SetLabelInput(mask);
  filter->Update();

  // Should I update also roi values (vol + density) ? No.
  double pixelVol = spect->GetSpacing()[0]*spect->GetSpacing()[1]*spect->GetSpacing()[2];
  double vol = filter->GetCount(1) * pixelVol * 0.001; // in CC

  // Store stats
  timeactivity.mean_counts_by_cc = filter->GetMean(1)/pixelVol; // mean counts by cc
  timeactivity.std_counts_by_cc = filter->GetSigma(1)/pixelVol; // std deviation by cc

  // Also peak here ? no. Not here : prefer for integrated activity)

  // Update db
  adb_->Update(timeactivity);
}
// --------------------------------------------------------------------
