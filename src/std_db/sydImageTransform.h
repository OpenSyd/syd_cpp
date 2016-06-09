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
#include "sydFile.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::ImageTransform") callback(Callback)
  /// Store information about a geometrical transformation from one image space to another.
  class ImageTransform :
    public syd::Record,
    public syd::RecordWithTags
  {
  public:

#pragma db not_null on_delete(cascade)
    /// Fixed image. Foreign key
    syd::Image::pointer fixed_image;

#pragma db not_null on_delete(cascade)
    /// Moving image. Foreign key
    syd::Image::pointer moving_image;

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
    TABLE_DEFINE(ImageTransform, syd::ImageTransform);
    // ------------------------------------------------------------------------

    /// Write the element as a string
    virtual std::string ToString() const;

    virtual void DumpInTable(syd::PrintTable2 & table) const;

    virtual void Callback(odb::callback_event, odb::database&) const;

    virtual std::string ComputeRelativeFolder() const;

    /// Check if the associated files exist on disk
    virtual syd::CheckResult Check() const;

  protected:
    ImageTransform();

  }; // end class
// --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
