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

#ifndef SYDIMAGESTITCH_H
#define SYDIMAGESTITCH_H

// syd
#include "sydImageCrop.h"


// --------------------------------------------------------------------
namespace syd {

  template<class ImageType>
    typename ImageType::Pointer
    StitchImages(const ImageType * s1, const ImageType * s2,
                 double threshold_cumul, double skip_slices);

#include "sydImageStitch.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
