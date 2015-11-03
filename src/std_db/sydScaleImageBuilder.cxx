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

// syd
#include "sydScaleImageBuilder.h"

// --------------------------------------------------------------------
void syd::ScaleImageBuilder::ScalePixelValue(syd::Image::pointer image, double s)
{

  if (image->pixel_type == "float")  ScalePixelValue<float>(image, s);
  else if (image->pixel_type == "short")  ScalePixelValue<short>(image, s);
  else if (image->pixel_type == "unsigned char")  ScalePixelValue<unsigned char>(image, s);
  else {
    LOG(FATAL) << "Unknown pixel_type: " << image->pixel_type;
  }
}
// --------------------------------------------------------------------
