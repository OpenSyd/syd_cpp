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

#ifndef SYDIMAGEGEOMETRICALMEAN_H
#define SYDIMAGEGEOMETRICALMEAN_H

#include "sydImageFlip.h"

#include <itkRecursiveGaussianImageFilter.h>

//--------------------------------------------------------------------
namespace syd {

  template<class ImageType>
  typename ImageType::Pointer
  GeometricalMean(const ImageType * ant_em,
                  const ImageType * post_em,
                  const ImageType * ant_sc,
                  const ImageType * post_sc,
                  double k);

  template<class ImageType>
  typename ImageType::Pointer
  RemoveScatter(const ImageType * em, const ImageType * sc, double k);

  template<class ImageType>
  typename ImageType::Pointer
  GeometricalMean(const ImageType * ant, const ImageType * post);

} // end namespace

#include "sydImageGeometricalMean.txx"
//--------------------------------------------------------------------

#endif
