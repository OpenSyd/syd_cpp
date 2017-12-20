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

#ifndef SYDFAFHELPER_H
#define SYDFAFHELPER_H

// syd
#include "sydImage.h"
#include "sydRoiMaskImageHelper.h"
#include "sydImageACF.h"

// --------------------------------------------------------------------
namespace syd {

  // Compute the attenuation map of an image.
  syd::Image::pointer InsertAttenuationCorrectionFactorImage(const syd::Image::pointer input,
                                                             const ACF_Parameters & p);

  // Compute the registered attenuation map of an image.
  syd::Image::pointer InsertRegisterPlanarSPECT(const syd::Image::pointer inputPlanar,
                                                const syd::Image::pointer inputSPECT,
                                                const syd::ImageProjection_Parameters & p,
                                                double & translation,
                                                std::string intermediate_result="");

  //Compute the FAF Integral
  double ComputeFAFIntegral(const syd::Image::pointer input_SPECT);

  // Compute the FAF calibrated of a SPECT image.
  syd::Image::pointer InsertFAFCalibratedImage(const syd::Image::pointer input_SPECT,
                                               const syd::Image::pointer input_planar,
                                               const syd::RoiMaskImage::pointer input_mask);


}
// --------------------------------------------------------------------

#endif
