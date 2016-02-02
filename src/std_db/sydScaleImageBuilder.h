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

#ifndef SYDSCALEIMAGEBUILDER_H
#define SYDSCALEIMAGEBUILDER_H

// syd
#include "sydImageBuilder.h"
#include "sydImageCrop.h"

// --------------------------------------------------------------------
namespace syd {

  /// Class to scale an image
  class ScaleImageBuilder: public syd::ImageBuilder {

  public:
    /// Constructor.
    ScaleImageBuilder(syd::StandardDatabase * db):ImageBuilder(db) {}

    /// Multiply the pixel values by a scalar
    void Scale(syd::Image::pointer image, double s);

  protected:
    /// templatized function, selected according to the pixel type
    template<class PixelType>
    void Scale(syd::Image::pointer image, double s);

  }; // class ScaleImageBuilder

#include "sydScaleImageBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
