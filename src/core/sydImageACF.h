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

#ifndef SYDIMAGEACF_H
#define SYDIMAGEACF_H

#include "sydImageProjection.h"

//--------------------------------------------------------------------
namespace syd
{
  // Option struct
  struct ACF_Parameters
  {
    int numberEnergySPECT;
    double attenuationWaterCT;
    double attenuationBoneCT;
    std::vector<double> attenuationAirSPECT;
    std::vector<double> attenuationWaterSPECT;
    std::vector<double> attenuationBoneSPECT;
    std::vector<double> weights;
    ImageProjection_Parameters proj;
  };

  // main function
  template<class InputImageType, class OutputImageType>
    typename OutputImageType::Pointer
    ComputeImageACF(const InputImageType * input, const ACF_Parameters & p);

} // end namespace

#include "sydImageACF.txx"
//--------------------------------------------------------------------

#endif
