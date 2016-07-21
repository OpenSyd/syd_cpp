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

#ifndef SYDIMAGEHELPER_H
#define SYDIMAGEHELPER_H

// syd
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This file contains helpers function that are hopefully helpful
  /// to create and update syd::Image table. All functions are static
  /// in a class for clarity.
  ///
  /// Example of use
  ///  syd::ImageHelper::CopyAndSetMhdImage(image, filename);
  class ImageHelper
  {
  public:

    static void CopyAndSetMhdImage(syd::Image::pointer image, std::string filename);

    static void CopyInformation(syd::Image::pointer image, const syd::Image::pointer like);

    static void InitializeEmptyMHDFiles(syd::Image::pointer image);

    static void SetPixelUnit(syd::Image::pointer image, std::string pixel_unit);

    static void UpdateImageProperties(syd::Image::pointer image);
    static void UpdateImageProperties(syd::Image::pointer image, itk::ImageIOBase::Pointer header);

    template<class ImageType>
    static void SetItkImage(syd::Image::pointer image,
                            typename ImageType::Pointer & itk_image);
  }; // end class
} // namespace syd

#include "sydImageHelper.txx"
// --------------------------------------------------------------------

#endif
