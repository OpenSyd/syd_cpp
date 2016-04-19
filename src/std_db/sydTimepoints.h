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

#ifndef SYDTIMEPOINTS_H
#define SYDTIMEPOINTS_H

// std
#include <array>

// syd
#include "sydRoiMaskImage.h"
#include "sydRecordWithTags.h"
#include "sydRecordWithHistory.h"
#include "sydRecordWithMD5Signature.h"
#include "sydTimeActivityCurve.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Timepoints") callback(Callback)
  /// Store information about a time activity curve (Timepoints).
  class Timepoints : public syd::Record,
                     public syd::RecordWithHistory,
                     public syd::RecordWithTags,
                     public syd::RecordWithMD5Signature {
  public:

#pragma db not_null
    /// Foreign key, it must exist in the Patient table
    syd::Patient::pointer patient;

#pragma db not_null
    /// Foreign key, it must exist in the Injection table (could be a 'fake' injection)
    syd::Injection::pointer injection;

    /// List of times
    std::vector<double> times;

    /// List values
    std::vector<double> values;

    /// Associated std dev (not required)
    std::vector<double> std_deviations;

    /// Associated images (not required)
    syd::Image::vector images;

    /// Associated mask (not required)
    syd::RoiMaskImage::pointer mask;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(Timepoints, syd::Timepoints);
    // ------------------------------------------------------------------------

    /// Write the element as a string
    virtual std::string ToString() const;

    /// Build a string to compute MD5
    virtual std::string ToStringForMD5() const;

    /// Callback (const)
    void Callback(odb::callback_event, odb::database&) const;

    /// Callback
    void Callback(odb::callback_event, odb::database&);

    /// Initialise a PrintTable
    virtual void InitTable(syd::PrintTable & table) const;

    /// Add a line in the given PrintTable
    virtual void DumpInTable(syd::PrintTable & table) const;

    /// Check if the history are ok
    virtual syd::CheckResult Check() const; //FIXME

    // Helper function, build a TAC
    void GetTAC(syd::TimeActivityCurve & tac);

  protected:
    Timepoints();

  }; // end class
  // --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif