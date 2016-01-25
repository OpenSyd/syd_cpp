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

#ifndef SYDCALIBRATION_H
#define SYDCALIBRATION_H

// std
#include <array>

// syd
#include "sydImage.h"
#include "sydTag.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Calibration") callback(Callback)


  /// Store information about a calibration factor to quantitatively interpret pixel in images.
  class Calibration : public syd::Record {
  public:

#pragma db not_null on_delete(cascade)
    /// Foreign key, it must exist in the Image table. If the image is deleted, the calibration also.
    syd::Image::pointer image;

    /// Associated tags
    syd::Tag::vector tags;

    /// Final calibration factor
    double factor;

    /// Intermediate information about fov ratio
    double fov_ratio;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(Calibration, syd::Calibration);
    TABLE_DECLARE_MANDATORY_FUNCTIONS(Calibration);
    TABLE_DECLARE_OPTIONAL_FUNCTIONS(Calibration);
    // ------------------------------------------------------------------------

    /// Add a tag to the list (check is already exist) ; do not update in the db.
    void AddTag(syd::Tag::pointer tag);

    /// Remove a tag from the list ; do not update in the db. Do nothing it not found
    void RemoveTag(syd::Tag::pointer tag);

    /// Callback : delete the associated files when the image is deleted.
    // void Callback(odb::callback_event, odb::database&) const;
    // void Callback(odb::callback_event, odb::database&);

    virtual void InitTable(syd::PrintTable & table) const;
    virtual void DumpInTable(syd::PrintTable & table) const;

  protected:
    Calibration();

  }; // end class
// --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
