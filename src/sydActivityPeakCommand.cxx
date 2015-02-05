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
#include "sydActivityPeakCommand.h"
#include "sydImage.h"

// --------------------------------------------------------------------
syd::ActivityPeakCommand::ActivityPeakCommand()
{
  set_mean_radius(5.0);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityPeakCommand::SetOptions(args_info_sydActivity & args_info)
{
  set_mean_radius(args_info.radius_arg);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityPeakCommand::Run(const Patient & patient,
                                   const RoiType & roitype,
                                   std::vector<std::string> & args)
{
  if (args.size() != 0) {
    std::string a;
    for (auto s:args) a += " "+s;
    LOG(WARNING) << "Warning, ignoring the options : " << a;
  }

  // Get or create the list of timeactivity
  std::vector<TimeActivity> timeactivities;
  std::vector<Timepoint> fake;
  GetOrCreateTimeActivities(patient, roitype, fake, timeactivities);

  // Loop to compute position and meanimage
  std::vector<ImageType::Pointer> spects;
  std::vector<ImageType::PointType> positions;
  for (auto ta:timeactivities) {
    // Load image
    Timepoint timepoint = sdb_->GetById<Timepoint>(ta.timepoint_id);
    RawImage ispect = sdb_->GetById<RawImage>(timepoint.spect_image_id);
    RoiMaskImage roi = sdb_->GetById<RoiMaskImage>(ta.roi_mask_image_id);
    std::string fspect = sdb_->GetImagePath(ispect);
    ImageType::Pointer spect = syd::ReadImage<ImageType>(fspect);

    // Load mask
    std::string fmask = sdb_->GetImagePath(roi);
    MaskImageType::Pointer mask = syd::ReadImage<MaskImageType>(fmask);

    // Crop spect like mask
    spect = syd::CropImageLike<ImageType>(spect, mask);
    //syd::WriteImage<ImageType>(spect, "spect-crop.mhd");

    // Compute mean (and keep image)
    spect = syd::MeanFilterImage<ImageType>(spect, mean_radius_);
    syd::WriteImage<ImageType>(spect, "mean.mhd");
    spects.push_back(spect);

    // Resample and crop the mask
    mask = syd::ResampleAndCropImageLike<MaskImageType>(mask, spect, 0, 0);

    // Compute max value position
    ImageType::PointType p = syd::GetMaxPosition<ImageType, MaskImageType>(spect, mask);
    ta.peak_position = PointToString(p);
    positions.push_back(p);
  }
  int nb = positions.size();

  // Compute mean & std position
  ImageType::PointType mean_position;
  ImageType::PointType std_position;
  mean_position.Fill(0.0);
  for(auto i=0; i<nb; i++)
    for(auto j=0; j<3; j++)
      mean_position[j] += positions[i][j];
  for(auto j=0; j<3; j++) mean_position[j] /= nb;
  for(auto i=0; i<nb; i++)
    for(auto j=0; j<3; j++)
      std_position[j] = pow(mean_position[j]-positions[i][j],2);
  for(auto j=0; j<3; j++) std_position[j] = sqrt(std_position[j]);

  // Store mean peak position
  Activity activity;
  bool b = adb_->GetIfExist<Activity>(odb::query<Activity>::patient_id == patient.id and
                                      odb::query<Activity>::roi_type_id == roitype.id, activity);
  if (!b) {
    activity = adb_->NewActivity(patient, roitype);
    ELOG(2) << "Creating activity object for patient " << patient.name;
  }
  activity.peak_mean_position = PointToString(mean_position);
  activity.peak_std_position = PointToString(std_position);
  adb_->Update(activity);

  // Loop to get value at max position
  for (auto i=0; i<nb; i++) {
    auto spect = spects[i];

    // Get (continuous, non integer) index
    itk::ContinuousIndex<double, 3> index;
    spect->TransformPhysicalPointToContinuousIndex(mean_position, index);

    // Create linear interpolator
    itk::LinearInterpolateImageFunction<ImageType, double>::Pointer interpolator =
      itk::LinearInterpolateImageFunction<ImageType, double>::New();
    interpolator->SetInputImage(spect);

    // Get value
    double v = interpolator->EvaluateAtContinuousIndex(index);
    double pixelVol = spect->GetSpacing()[0]*spect->GetSpacing()[1]*spect->GetSpacing()[2];
    timeactivities[i].peak_counts_by_mm3 = v / pixelVol;
  }

  // Update db
  for (auto ta:timeactivities) adb_->Update(ta);

  // Verbose
  for (auto i=0; i<nb; i++) {
    TimeActivity ta = timeactivities[i];
    Timepoint timepoint = sdb_->GetById<Timepoint>(ta.timepoint_id);
    ELOG(1) << patient.synfrizz_id << " " << patient.name << " " << roitype.name << " "
            << timepoint.number  << " \t"
            << ta.peak_counts_by_mm3 << " \t " << ta.peak_position << " "
            << activity.peak_mean_position;
  }
}
// --------------------------------------------------------------------
