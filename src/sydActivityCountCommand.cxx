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
#include "sydActivityCountCommand.h"
#include "sydImage.h"

// --------------------------------------------------------------------
syd::ActivityCountCommand::ActivityCountCommand()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCountCommand::SetOptions(args_info_sydActivity & args_info)
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCountCommand::Run(const Patient & patient,
                                    const RoiType & roitype,
                                    std::vector<std::string> & args)
{
  // args for the timepoint
  std::vector<Timepoint> timepoints;
  if (args.size() == 0) args.push_back("all");
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
      LOG(FATAL) << "Error I could not find timepoint number " << n
                 << " for the patient " << patient.name;
    }
    timepoints.push_back(t);
  }

  // Get or create the list of timeactivity
  std::vector<TimeActivity> timeactivities;
  GetOrCreateTimeActivities(patient, roitype, timepoints, timeactivities);

  // Run computation
  for(auto ta:timeactivities) Run(ta);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityCountCommand::Run(TimeActivity & timeactivity)
{
  // Get corresponding timepoint and roi
  Timepoint timepoint(sdb_->GetById<Timepoint>(timeactivity.timepoint_id));
  RoiMaskImage roi(sdb_->GetById<RoiMaskImage>(timeactivity.roi_mask_image_id));

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
  mask = syd::ResampleAndCropImageLike<MaskImageType>(mask, spect, 0, 0);

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
  timeactivity.mean_counts_by_mm3 = filter->GetMean(1)/pixelVol; // mean counts by mm3
  timeactivity.std_counts_by_mm3 = filter->GetSigma(1)/pixelVol; // std deviation by mm3

  // Also peak here ? no. Not here : prefer for integrated activity)

  // Update db
  adb_->Update(timeactivity);

  // Verbose
  Patient patient = cdb_->GetById<Patient>(timepoint.patient_id);
  RoiType roitype = cdb_->GetById<RoiType>(roi.roitype_id);
  ELOG(1) << patient.synfrizz_id << " " << patient.name << " " << roitype.name << " "
          << timepoint.number  << " \t"
          << timeactivity.mean_counts_by_mm3 << " \t " << timeactivity.std_counts_by_mm3;
}
// --------------------------------------------------------------------
