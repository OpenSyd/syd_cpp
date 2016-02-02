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
template<class PixelType>
void syd::ScaleImageBuilder::Scale(syd::Image::pointer image, double s)
{
  // load itk image
  typedef itk::Image<PixelType,3> ImageType;
  typename ImageType::Pointer itk_image =
    syd::ReadImage<ImageType>(db_->GetAbsolutePath(image));

  // Perform scale
  syd::ScaleImage<ImageType>(itk_image, s);

  // Update image
  SetImage<PixelType>(image, itk_image);
}
// --------------------------------------------------------------------
