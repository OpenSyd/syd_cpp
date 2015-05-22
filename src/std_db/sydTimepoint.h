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

// --------------------------------------------------------------------
namespace syd {

#pragma db object
  /// Store information about a timepoint.
  class Timepoint : public syd::TableElement {
  public:

#pragma db id auto
    /// Id of the Timepoint
    IdType id;

    /// Foreign key, it must exist in the Patient table
    // -> get patient from injection
    //std::shared_ptr<syd::Patient> patient_;

    /// FIXME
    std::shared_ptr<syd::Tag> tag;
    std::shared_ptr<syd::Injection> injection;
    double time_from_injection_in_hours;
    std::vector<std::shared_ptr<syd::DicomSerie>> dicoms;
    //    std::vector<std::shared_ptr<syd::Image>> images;

    // --------------------------------------------------
    SET_TABLE_NAME("Timepoint")
    Timepoint();
    ~Timepoint(); // FIXME
    Timepoint(const Timepoint & other);
    Timepoint & operator= (const Timepoint & other);
    void copy(const Timepoint & t);
    virtual std::string ToString() const;
    virtual void SetValues(std::vector<std::string> & arg);

    bool operator==(const Timepoint & p);
    bool operator!=(const Timepoint & p) { return !(*this == p); }

  }; // end class
}
// --------------------------------------------------------------------

#endif
