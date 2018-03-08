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

#ifndef SYDATTENUATIONIMAGE_H
#define SYDATTENUATIONIMAGE_H

#include <math.h>

// syd
#include "sydCommon.h"
#include "sydFileUtils.h"

// itk
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

// --------------------------------------------------------------------
namespace syd {

  //Main function to compute the attenuation image
  template<class ImageType>
  typename ImageType::Pointer
  AttenuationImage(const ImageType * input, double numberEnergySPECT,
                   double attenuationWaterCT, double attenuationBoneCT,
                   std::vector<double>& attenuationAirSPECT,
                   std::vector<double>& attenuationWaterSPECT,
                   std::vector<double>& attenuationBoneSPECT,
                   std::vector<double>& weight);

} // end namespace

#include "sydAttenuationImage.txx"

// --------------------------------------------------------------------

#endif
