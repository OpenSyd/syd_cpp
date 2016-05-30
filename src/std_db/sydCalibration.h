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
#include "sydRecordWithTags.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Calibration") callback(Callback)

  /// Store information about a calibration factor to quantitatively
  /// interpret pixel in images.
  class Calibration : public syd::Record,
                      public syd::RecordWithTags {
  public:

#pragma db not_null on_delete(cascade)
    /// Foreign key, it must exist in the Image table. If the image is
    /// deleted, the calibration also.
    syd::Image::pointer image;

    /// Final calibration factor
    double factor;

    /// Intermediate information about fov ratio
    double fov_ratio;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(Calibration, syd::Calibration);
    // ------------------------------------------------------------------------

    /// Write the element as a string
    virtual std::string ToString() const;

    virtual void InitTable(syd::PrintTable & table) const;
    virtual void DumpInTable(syd::PrintTable & table) const;

  protected:
    Calibration();

  }; // end class
// --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
