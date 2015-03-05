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
#include "sydActivityDoseCommand.h"
#include "sydImage.h"
#include "sydActivityLambdaCommand.h"
#include "sydTimeActivityCurveIntegrate.h"

// --------------------------------------------------------------------
syd::ActivityDoseCommand::ActivityDoseCommand()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDoseCommand::SetOptions(args_info_sydActivity & args_info)
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDoseCommand::Run(const Patient & patient,
                                   const RoiType & roitype,
                                   std::vector<std::string> & args)
{
  // Get or create the activity
  Activity activity;
  bool b = adb_->GetIfExist<Activity>(odb::query<Activity>::patient_id == patient.id and
                                      odb::query<Activity>::roi_type_id == roitype.id, activity);
  if (!b) activity = adb_->NewActivity(patient, roitype);

  // Retrive the dose + uncertainty image
  std::string tag = "dose";
  RawImage dose;
  b = sdb_->GetIfExist<RawImage>(odb::query<RawImage>::patient_id == patient.id and
                                odb::query<RawImage>::tag == tag, dose);
  if (!b) {
    LOG(WARNING) << "Could not find 'dose' map for patient " << patient.name << ". I do nothing. ";
    return;
  }
  tag = "uncertainty";
  RawImage uncer;
  b = sdb_->GetIfExist<RawImage>(odb::query<RawImage>::patient_id == patient.id and
                                 odb::query<RawImage>::tag == tag, uncer);
  if (!b) {
    LOG(WARNING) << "Could not find 'uncertainty' dose map for patient " << patient.name << ". I do nothing. ";
    return;
  }

  // Get the ROI
  std::vector<Timepoint> timepoints;
  sdb_->GetTimepoints(patient, timepoints);
  if (timepoints.size() < 1) {
    LOG(WARNING) << "Could not find any timepoints for patient " << patient.name << ". I do nothing. ";
    return;
  }
  RoiMaskImage roi;
  b = sdb_->GetIfExist<RoiMaskImage>(odb::query<RoiMaskImage>::timepoint_id == timepoints[0].id and
                                     odb::query<RoiMaskImage>::roitype_id == roitype.id, roi);
  if (!b) {
    return; // no warning
  }

  // Read the images
  ImageType::Pointer img_dose = syd::ReadImage<ImageType>(sdb_->GetImagePath(dose));
  ImageType::Pointer img_uncert = syd::ReadImage<ImageType>(sdb_->GetImagePath(uncer));
  MaskImageType::Pointer img_roi = syd::ReadImage<MaskImageType>(sdb_->GetImagePath(roi));

  // Compute the mean dose + uncertainty
  img_roi = syd::ResampleAndCropImageLike<MaskImageType>(img_roi, img_dose, 0, 0);
  typedef itk::LabelStatisticsImageFilter<ImageType, MaskImageType> FilterType;
  typename FilterType::Pointer filter=FilterType::New();
  filter->SetInput(img_dose);
  filter->SetLabelInput(img_roi);
  filter->Update();
  double mean_dose = filter->GetMean(1);
  filter->SetInput(img_uncert);
  filter->SetLabelInput(img_roi);
  filter->Update();
  double mean_uncert = filter->GetMean(1);

  // Store values
  activity.mean_dose = mean_dose;
  activity.mean_uncertainty = mean_uncert;
  adb_->Update(activity);

  // output
  ELOG(1) << "Mean dose for " << patient.name << " in " << roitype.name << " is " << mean_dose
          << " Gy/inject_MBq. Uncertainty is " << mean_uncert*100 << "%.";
}
// --------------------------------------------------------------------
