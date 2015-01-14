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
  class TimeActivity
  {
  public:

#pragma db id auto
    IdType        id;
    IdType        timepoint_id;
    IdType        roi_mask_image_id;
    IdType        patient_id;
    double        mean_counts_by_mm3;
    double        std_counts_by_mm3;
    double        peak_counts_by_mm3;
    std::string   peak_position;

    // --------------------------------------------------
    TimeActivity() {}
    ~TimeActivity() {}
    TimeActivity(const TimeActivity & other) { copy(other); }
    // --------------------------------------------------


    // --------------------------------------------------
    TimeActivity & operator= (const TimeActivity & other) {
      if (this != &other) { copy(other); }
      return *this;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const TimeActivity & p) {
      os << p.id << " " << p.timepoint_id << " " << p.roi_mask_image_id << " "
         << p.patient_id << " "
         << p.mean_counts_by_mm3 << " " << p.std_counts_by_mm3 << " "
         << p.peak_counts_by_mm3 << " " << p.peak_position;
      return os;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    void copy(const TimeActivity & t) {
      id = t.id;
      timepoint_id = t.timepoint_id;
      roi_mask_image_id = t.roi_mask_image_id;
      patient_id = t.patient_id;
      mean_counts_by_mm3 = t.mean_counts_by_mm3;
      std_counts_by_mm3 = t.std_counts_by_mm3;
      peak_counts_by_mm3 = t.peak_counts_by_mm3;
      peak_position = t.peak_position;
    }
    // --------------------------------------------------

  }; // class TimeActivity
} // namespace syd
// --------------------------------------------------------------------
