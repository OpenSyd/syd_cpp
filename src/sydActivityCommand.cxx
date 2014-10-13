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
  DDS(patients);
  args.erase(args.begin());
  for(auto p:patients) {
    DD(p);
    Run(p, args);
  }
  DD("done");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommand::Run(const Patient & patient, std::vector<std::string> & args)
{
  std::vector<Timepoint> timepoints;
  std::string s = args[0];
  if (s == "all") {
    sdb_->LoadVector<Timepoint>(timepoints, odb::query<Timepoint>::patient_id == patient.id);
  }
  else {
    int n = atoi(s.c_str());
    DD(n);
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
  DD(patient);

  // find all roitypes
  std::string roiname = args[0];
  DD(roiname);
  std::vector<RoiType> roitypes;
  cdb_->LoadVector<RoiType>(roitypes, odb::query<RoiType>::name.like(roiname));
  DDS(roitypes);

  // Get corresponding roimaskimage
  std::vector<RoiMaskImage> rois;
  for(auto rt:roitypes) {
    RoiMaskImage roi;
    bool b = sdb_->GetIfExist<RoiMaskImage>(odb::query<RoiMaskImage>::timepoint_id == timepoint.id and
                                            odb::query<RoiMaskImage>::roitype_id == rt.id, roi);
    if (b) rois.push_back(roi);
  }

  // Loop on rois
  for(auto r:rois) {
    DD(r);
    TimeActivity timeactivity;
    RoiType roitype = sdb_->GetById<RoiType>(r.roitype_id);
    DD(roitype);
    bool b = adb_->GetIfExist<TimeActivity>(odb::query<TimeActivity>::timepoint_id == timepoint.id and
                                            odb::query<TimeActivity>::roi_id == r.id,
                                            timeactivity);
    if (!b) {
      VLOG(1) << "Creating new timeactivity for " << patient.name << " tp="
              << timepoint.number << " roi= " <<  roitype.name;
      timeactivity = adb_->NewTimeActivity(timepoint, r);
    }
    DD(timeactivity);
    UpdateActivityInRoi(timepoint, r, timeactivity);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCommand::UpdateActivityInRoi(const Timepoint & timepoint,
                                               const RoiMaskImage & roi,
                                               TimeActivity & timeactivity)
{
  DD("UpdateActivityInRoi");
  DD(timepoint);
  DD(roi);
  DD(timeactivity);

  // update fields (?) --> no check ! (FIXME)
  //  timeactivity.timepoint_id = timepoint.id;
  //timeactivity.roi_id = roi.id;

  DD("to put in UpdateActivity");

  /*
  // load spect image
  RawImage spect = adb_->GetById<RawImage>(timepoint.spect_image_id);
  std::string fspect = adb_->GetImagePath(spect);
  DD(fspect);
  ImageType::Pointer spect = syd::ReadImage<ImageType>(fspect);

  // load roi mask
  std::string fmask = adb_->GetImagePath(roi);
  DD(fmask);
  MaskImageType::Pointer mask = syd::ReadImage<MaskImageType>(fmask);

  // in general spect and mask not the same spacing. Need to resample
  // one of the two. FIXME

  // resample mask like the spect image (both spacing and crop)
  mask = syd::ResampleImageLike<MaskImageType>(mask, spect, 0, 0);

  //FIXME or reverse ... --> but need to store correct volume for the counts
  // if resample, need to convert values in counts_concentration

  // compute stats
  typedef itk::LabelStatisticsImageFilter<ImageType, MaskImageType> FilterType;
  typename FilterType::Pointer filter=FilterType::New();
  filter->SetInput(spect);
  filter->SetLabelInput(mask);
  filter->Update();

  // Should I update also roi values (vol + density ?) FIXME
  double pixelVol = spect->GetSpacing()[0]*spect->GetSpacing()[1]*spect->GetSpacing()[2];
  DD(pixelVol);
  DD(filter->GetCount(1));
  double vol = filter->GetCount(1) * pixelVol * 0.001; // in CC
  DD(vol);

  // Store stats
  timeactivity.counts_by_cc = filter->GetSum(1)/vol; // sum of counts in the roi
  timeactivity.std_counts = filter->GetSigma(1)/vol; // std deviation of the counts by pixels

  // Also peak here FIXME

  // Update db
  adb_->Update(timeactivity);
  */
}
// --------------------------------------------------------------------
