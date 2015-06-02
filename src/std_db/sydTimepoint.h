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

#ifndef SYDTIMEPOINT_H
#define SYDTIMEPOINT_H

// syd
#include "sydDicomSerie.h"
#include "sydTag.h"
#include "sydImage.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object
  /// Store information about a timepoint.
  class Timepoint : public syd::TableElementBase {
  public:

#pragma db id auto
    /// Id of the Timepoint
    IdType id;

    /// Associated tag. The Timepoint is deleted if the tag is
    /// deleted.
#pragma db on_delete(cascade) // FIXME
    std::shared_ptr<syd::Tag> tag;

    /// Associated injection. The Timepoint is deleted if the
    /// injection is deleted.
#pragma db on_delete(cascade) // FIXME
    std::shared_ptr<syd::Injection> injection;

    /// Time from the injection (in hours) of the first dicoms.
    double time_from_injection_in_hours;

    /// List of associated dicoms. In general one CT and one SPECT (or
    /// PET), but several SPECT bed position image can be associated.
    std::vector<std::shared_ptr<syd::DicomSerie>> dicoms; // FIXME

    std::vector<std::shared_ptr<syd::Image>> images; // FIXME

    // --------------------------------------------------
    SET_TABLE_NAME("Timepoint")
    Timepoint();

    virtual std::string ToString() const;
    virtual void SetValues(std::vector<std::string> & arg);

    bool operator==(const Timepoint & p);
    bool operator!=(const Timepoint & p) { return !(*this == p); }

  }; // end class
}
// --------------------------------------------------------------------

#endif
