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

// std
#include <string>
#include <iostream>
#include <memory>

// syd
#include "sydCommon.h"

// odb
#include <odb/core.hxx>
#include <odb/tr1/memory.hxx>

using std::tr1::shared_ptr;

// --------------------------------------------------------------------
namespace syd {
#pragma db object
  class Timepoint
  {
  public:

#pragma db id auto
    IdType        id;
    IdType        patient_id;      // not necessary but useful. keep it.
    IdType        spect_serie_id;
    IdType        ct_serie_id;
    unsigned int  number;
    double        time_from_injection_in_hours;
    IdType        ct_image_id;
    IdType        spect_image_id;

    // --------------------------------------------------
    Timepoint() {}
    ~Timepoint() {}
    Timepoint(const Timepoint & other) { copy(other); }
    // --------------------------------------------------


    // --------------------------------------------------
    Timepoint & operator= (const Timepoint & other) {
      if (this != &other) { copy(other); }
      return *this;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const Timepoint & p) {
      os << p.id << " " << p.number << " " << p.time_from_injection_in_hours
         << " spect=" << p.spect_image_id
         << " ct=" << p.ct_image_id;
      return os;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    void copy(const Timepoint & t) {
      id = t.id;
      patient_id = t.patient_id;
      spect_serie_id = t.spect_serie_id;
      ct_serie_id = t.ct_serie_id;
      number = t.number;
      time_from_injection_in_hours = t.time_from_injection_in_hours;
      ct_image_id = t.ct_image_id;
      spect_image_id = t.spect_image_id;
    }
    // --------------------------------------------------

  }; // class Timepoint
} // namespace syd
// --------------------------------------------------------------------
