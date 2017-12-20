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

#ifndef SYDIMAGEPROJECTION_H
#define SYDIMAGEPROJECTION_H


//--------------------------------------------------------------------
namespace syd {

  // Option struct
  struct ImageProjection_Parameters
  {
    int projectionDimension;
    bool flipProjectionFlag;
    bool meanFlag = false;
  };

  template<class ImageType, class OutputImageType>
  typename OutputImageType::Pointer
    Projection(const ImageType * input, const ImageProjection_Parameters & p);

  template<class ImageType, class OutputImageType>
  typename OutputImageType::Pointer
  Projection(const ImageType * input, double dimension);

} // end namespace

#include "sydImageProjection.txx"
//--------------------------------------------------------------------

#endif
