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

#ifndef SYDFAFMASK_H
#define SYDFAFMASK_H

#include "sydImageProjection.h"

//--------------------------------------------------------------------
namespace syd {

  template<class ImageType2D, class ImageType3D>
  typename ImageType2D::Pointer
    FAFMask(const ImageType3D * input_SPECT,
            const ImageType2D * input_planar,
            const ImageProjection_Parameters & p);

} // end namespace

#include "sydFAFMask.txx"
//--------------------------------------------------------------------

#endif
