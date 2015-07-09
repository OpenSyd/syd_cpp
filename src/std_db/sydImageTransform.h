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

#ifndef SYDIMAGETRANSFORM_H
#define SYDIMAGETRANSFORM_H

// syd
#include "sydRoiMaskImage.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::ImageTransform")
  /// Store information about a geometrical transformation from one image space to another.
  class ImageTransform : public syd::Record {
  public:

#pragma db not_null on_delete(cascade)
    /// Fixed image. Foreign key
    syd::Image::pointer fixed_image;

#pragma db not_null on_delete(cascade)
    /// Moving image. Foreign key
    syd::Image::pointer moving_image;

    /// Associated tags
    syd::Tag::vector tags;

    /// Mask of fixed image. Can be null.
    syd::RoiMaskImage::pointer fixed_mask;

    /// Mask of moving image.Can be null.
    syd::RoiMaskImage::pointer moving_mask;

#pragma db on_delete(cascade)
    /// Elastix config file
    syd::File::pointer config_file;

#pragma db on_delete(cascade)
    /// Result file
    syd::File::pointer transform_file;

    /// Date of the computation
    std::string date;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(ImageTransform);
    TABLE_DECLARE_MANDATORY_FUNCTIONS(ImageTransform);
    TABLE_DECLARE_OPTIONAL_FUNCTIONS(ImageTransform);
    // ------------------------------------------------------------------------

  }; // end class
// --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
