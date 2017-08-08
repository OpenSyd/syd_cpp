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

#ifndef SYDIMAGEFLIP_H
#define SYDIMAGEFLIP_H

// syd
#include "sydCommon.h"
#include "sydFileUtils.h"

// itk
#include <itkFlipImageFilter.h>

// --------------------------------------------------------------------
namespace syd {

  template<class ImageType>
  typename ImageType::Pointer FlipImage(const ImageType * input, int axe, bool flipOrigin=false);

  template<class ImageType>
  bool FlipImageIfNegativeSpacing(typename ImageType::Pointer & image);

} // end namespace

#include "sydImageFlip.txx"

// --------------------------------------------------------------------

#endif
