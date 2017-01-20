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


//--------------------------------------------------------------------
namespace syd {

  /* This function consider all pixels in the input image that are in
     the mask image. They replace their value by the mean value of a
     neighborhood, provided that the neighboring pixels are not in the
     mask and does not have value at zero.
  */

  template<class ImageType>
    void FillHoles(ImageType * input,
                  const ImageType * mask,
                  int r,
                  double foreground,
                  int & nb_failures,
                  int & nb_changed);

#include "sydImageFillHoles.txx"

} // end namespace
//--------------------------------------------------------------------
