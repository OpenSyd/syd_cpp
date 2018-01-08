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
#include "sydRoiMaskImageHelper.h"
#include "sydImageACF.h"
#include "sydFAFCalibratedImage.h"
#include "sydRegisterPlanarSPECT.h"
#include "sydAttenuationCorrectedPlanarImage.h"
#include "sydFAFMask.h"

// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertAttenuationCorrectionFactorImage(const syd::Image::pointer input,
                                            const syd::ACF_Parameters & p)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::Image<PixelType, 2> OutputImageType;
  auto itk_input = syd::ReadImage<ImageType>(input->GetAbsolutePath());
  auto attenuation = syd::ComputeImageACF<ImageType, OutputImageType>(itk_input, p);

  // Create the syd image
  return syd::InsertImage<OutputImageType>(attenuation, input->patient, input->modality);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertRegisterPlanarSPECT(const syd::Image::pointer inputPlanar,
                               const syd::Image::pointer inputSPECT,
                               const syd::ImageProjection_Parameters & p,
                               double & translation,
                               std::string intermediate_result)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 2> ImageType2D;
  typedef itk::Image<PixelType, 3> ImageType3D;
  auto itk_inputPlanar = syd::ReadImage<ImageType2D>(inputPlanar->GetAbsolutePath());
  auto itk_inputSPECT = syd::ReadImage<ImageType3D>(inputSPECT->GetAbsolutePath());
  auto projectionSPECT =
    syd::Projection<ImageType3D, ImageType2D>(itk_inputSPECT, p);

  auto imageRegister = syd::RegisterPlanarSPECT<ImageType2D>(itk_inputPlanar,
                                                             projectionSPECT,
                                                             translation);
  if (intermediate_result != "") {
    syd::WriteImage<ImageType2D>(itk_inputPlanar, intermediate_result+"_input1.mhd");
    syd::WriteImage<ImageType2D>(projectionSPECT, intermediate_result+"_input2.mhd");
    syd::WriteImage<ImageType2D>(imageRegister, intermediate_result+"_output.mhd");
  }

  // Insert a new image
  auto copy = syd::InsertCopyImage(inputPlanar);
  syd::WriteImage<ImageType2D>(imageRegister, copy->GetAbsolutePath(), copy->dimension);
  copy->frame_of_reference_uid = inputSPECT->frame_of_reference_uid;
  return copy;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertAttenuationCorrectedPlanarImage(const syd::Image::pointer input_GM,
                                           const syd::Image::pointer input_ACF,
                                           double outside_factor)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 2> ImageType2D;
  auto itk_input_GM = syd::ReadImage<ImageType2D>(input_GM->GetAbsolutePath());
  auto itk_input_ACF = syd::ReadImage<ImageType2D>(input_ACF->GetAbsolutePath());
  auto attenuationCorrected = syd::AttenuationCorrectedPlanarImage<ImageType2D>(itk_input_GM, itk_input_ACF, outside_factor);

  // Create the syd image
  return syd::InsertImage<ImageType2D>(attenuationCorrected, input_GM->patient, input_GM->modality);
}
// --------------------------------------------------------------------


//--------------------------------------------------------------------
double syd::ComputeFAFIntegral(const syd::Image::pointer input_SPECT, int nb_of_heads)
{
  if (input_SPECT->injection == nullptr) {
    EXCEPTION("In FAF Calibration: no injection associated to " <<  input_SPECT);
  }

  //injected activity in MBq
  double injectedActivity = input_SPECT->injection->activity_in_MBq;
  DD(injectedActivity);
  //lambda decay in 1/s
  double lambdaDecay = input_SPECT->injection->GetLambdaDecayConstantInHours()/3600.0;
  DD(lambdaDecay);
  //Time between injection and the beginning of the SPECT acquisition in s
  double timeInjectionSPECT = input_SPECT->GetHoursFromInjection()*3600.0;
  DD(timeInjectionSPECT);
  // Acquisition duration
  DD(input_SPECT->dicoms[0]->dicom_actual_frame_duration_in_msec);
  DD(input_SPECT->dicoms[0]->dicom_number_of_frames_in_rotation);
  DD(input_SPECT->dicoms[0]->dicom_number_of_rotations);
  double totalAcquisitionDuration = input_SPECT->dicoms[0]->dicom_actual_frame_duration_in_msec/1000.0*
    input_SPECT->dicoms[0]->dicom_number_of_frames_in_rotation/nb_of_heads*input_SPECT->dicoms[0]->dicom_number_of_rotations;
  //Total acquisition time in s (for 4 heads) FIXME
  DD(totalAcquisitionDuration);

  //Compute A0
  double A0 = injectedActivity*std::exp(-lambdaDecay*timeInjectionSPECT);
  DD(A0);

  //Compute the integral between 0 and totalAcquisitionDuration of exp(-lambdaDecay*t)
  double integral = (1 - std::exp(-lambdaDecay*totalAcquisitionDuration))/lambdaDecay;
  DD(integral);

  return (A0*integral);
}
//--------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer
syd::InsertFAFMask(const syd::Image::pointer input_SPECT,
                   const syd::Image::pointer input_planar,
                   syd::ImageProjection_Parameters & p)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType3D;
  typedef itk::Image<PixelType, 2> ImageType2D;
  auto itk_input_SPECT = syd::ReadImage<ImageType3D>(input_SPECT->GetAbsolutePath());
  auto itk_input_planar = syd::ReadImage<ImageType2D>(input_planar->GetAbsolutePath());
  auto fafMask = syd::FAFMask<ImageType2D, ImageType3D>(itk_input_SPECT, itk_input_planar, p);

  // Create the syd image
  auto db = input_planar->patient->GetDatabase<syd::StandardDatabase>();
  auto roiType = syd::FindRoiType("FAF", db);
  return syd::InsertRoiMask<ImageType2D>(fafMask, input_planar->patient, roiType);
}
// --------------------------------------------------------------------




// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertFAFCalibratedImage(const syd::Image::pointer input_SPECT,
                              const syd::Image::pointer input_planar,
                              int nb_of_heads,
                              syd::ImageProjection_Parameters & p)
{
  // Compute the mask
  auto input_mask = syd::InsertFAFMask(input_SPECT, input_planar, p);

  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 2> ImageType2D;
  typedef itk::Image<PixelType, 3> ImageType3D;
  auto itk_input_SPECT = syd::ReadImage<ImageType3D>(input_SPECT->GetAbsolutePath());
  auto itk_input_planar = syd::ReadImage<ImageType2D>(input_planar->GetAbsolutePath());
  auto itk_input_mask = syd::ReadImage<ImageType2D>(input_mask->GetAbsolutePath());
  double integral = syd::ComputeFAFIntegral(input_SPECT, nb_of_heads);
  double f = 1.0;
  auto fafCalibrated = syd::FAFCalibratedImage<ImageType2D, ImageType3D>(itk_input_SPECT,
                                                                         itk_input_planar,
                                                                         itk_input_mask,
                                                                         integral, f);
  // Create the syd image
  auto faf = syd::InsertImage<ImageType3D>(fafCalibrated, input_SPECT->patient, input_SPECT->modality);
  syd::SetImageInfoFromImage(faf, input_SPECT);
  auto db = input_SPECT->GetDatabase<syd::StandardDatabase>();
  faf->pixel_unit = syd::FindOrCreatePixelUnit(db, "MBq/mm3", "FAF calibrated image unit");
  syd::SetPixelUnit(faf, "MBq/mm3");
  std::ostringstream oss;
  oss << "FAF = " << f;
  faf->comments.push_back(oss.str());
  db->Update(faf);

  // Remove FAF mask
  // db->Delete(input_mask);

  // end
  return faf;
}
// --------------------------------------------------------------------
