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

#ifndef SYDELASTIX_H
#define SYDELASTIX_H

// syd
#include "sydRoiMaskImage.h"
#include "sydFile.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Elastix") callback(Callback)
  /// Store information about a geometrical transformation from one image space to another.
  class Elastix :
    public syd::Record,
    public syd::RecordWithHistory,
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

    // ------------------------------------------------------------------------
    TABLE_DEFINE(Elastix, syd::Elastix);
    // ------------------------------------------------------------------------

    /// Write the element as a string
    virtual std::string ToString() const;

    /// Add a line in the given PrintTable
    virtual void DumpInTable(syd::PrintTable & table) const;
    virtual void DumpInTable_default(syd::PrintTable & table) const;
    virtual void DumpInTable_history(syd::PrintTable & table) const;

    virtual void Callback(odb::callback_event, odb::database&) const;

    /// Check if the associated files exist on disk
    virtual syd::CheckResult Check() const;

    /// Compute the default result path (based on the patient's name)
    virtual std::string ComputeDefaultFolder();

    /// Compute the default result filename: always "TransformParameters.0.txt"
    virtual std::string ComputeDefaultFilename();

  protected:
    Elastix();

  }; // end class
  // --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
