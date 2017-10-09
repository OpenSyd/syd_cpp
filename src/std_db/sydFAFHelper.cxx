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
#include "sydFAFHelper.h"
#include "sydFAFCalibratedImage.h"

//--------------------------------------------------------------------
double syd::ComputeFafIntegral(const syd::Image::pointer input_SPECT)
{
  //Get the different important value
  double injectedActivity = input_SPECT->injection->activity_in_MBq; //injected activity in MBq
  double lambdaDecay = input_SPECT->injection->GetLambdaDecayConstantInHours()/3600.0; //lambda decay in 1/s
  double timeInjectionSPECT = input_SPECT->GetHoursFromInjection()*3600.0; //Time between injection and the beginning of the SPECT acquisition in s
  double totalAcquisitionTime = input_SPECT->dicoms[0]->dicom_actual_frame_duration_in_msec/1000.0*input_SPECT->dicoms[0]->dicom_number_of_frames_in_rotation/4*input_SPECT->dicoms[0]->dicom_number_of_rotations; //Total acquisition time in s (for 4 heads)

  //Compute A0
  double A0 = injectedActivity*std::exp(-lambdaDecay*timeInjectionSPECT);

  //Compute the integral between 0 and totalAcquisitionTime of exp(-lambdaDecay*t)
  double integral = (1 - std::exp(-lambdaDecay*totalAcquisitionTime))/lambdaDecay;

  return (A0*integral);
}
//--------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertFAFCalibratedImage(const syd::Image::pointer input_SPECT,
                              const syd::Image::pointer input_planar,
                              const syd::RoiMaskImage::pointer input_mask)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 2> ImageType2D;
  typedef itk::Image<PixelType, 3> ImageType3D;
  auto itk_input_SPECT = syd::ReadImage<ImageType3D>(input_SPECT->GetAbsolutePath());
  auto itk_input_planar = syd::ReadImage<ImageType2D>(input_planar->GetAbsolutePath());
  auto itk_input_mask = syd::ReadImage<ImageType2D>(input_mask->GetAbsolutePath());
  double integral = syd::ComputeFafIntegral(input_SPECT);
  auto fafCalibrated = syd::FAFCalibratedImage<ImageType2D, ImageType3D>(itk_input_SPECT, itk_input_planar, itk_input_mask, integral);

  // Create the syd image
  auto faf = syd::InsertImage<ImageType3D>(fafCalibrated, input_SPECT->patient, input_SPECT->modality);
  syd::SetImageInfoFromImage(faf, input_SPECT);
  auto db = input_SPECT->GetDatabase<syd::StandardDatabase>();
  faf->pixel_unit = syd::FindOrCreatePixelUnit(db, "MBq"); //FIXME
  db->Update(faf);
  return faf;
}
// --------------------------------------------------------------------
