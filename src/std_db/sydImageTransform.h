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

#pragma db object
  /// Store information about a geometrical transformation from one image space to another.
  class ImageTransform : public syd::TableElementBase {
  public:

#pragma db id auto
    /// Id of the ImageTransform
    IdType id;

#pragma db not_null
    /// Fixed image. Foreign key
    std::shared_ptr<syd::Image> fixed_image;

#pragma db not_null
    /// Moving image. Foreign key
    std::shared_ptr<syd::Image> moving_image;

    /// Associated tags
    std::vector<std::shared_ptr<syd::Tag>> tags;

    /// Mask of fixed image.
    std::shared_ptr<syd::RoiMaskImage> fixed_mask;

    /// Mask of moving image
    std::shared_ptr<syd::RoiMaskImage> moving_mask;

    /// Elastix config file
    std::shared_ptr<syd::File> config_file;

    /// Result file
    std::shared_ptr<syd::File> transform_file;

    /// Date of the computation
    std::string date;

    // ------------------------------------------------------------------------
    SET_TABLE_NAME("ImageTransform")
    ImageTransform();

    virtual std::string ToString() const;
    virtual std::string ToLargeString() const;

    bool operator==(const ImageTransform & p);
    bool operator!=(const ImageTransform & p) { return !(*this == p); }

    virtual void OnDelete(syd::Database * db);

  }; // end class
// --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
