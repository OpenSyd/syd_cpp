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

// --------------------------------------------------------------------
namespace syd {
#pragma db object
  class Activity
  {
  public:

#pragma db id auto
    IdType        id;
    IdType        patient_id;
    IdType        average_ct_image_id;

    // --------------------------------------------------
    Activity() {}
    ~Activity() {}
    Activity(const Activity & other) { copy(other); }
    // --------------------------------------------------


    // --------------------------------------------------
    Activity & operator= (const Activity & other) {
      if (this != &other) { copy(other); }
      return *this;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const Activity & p) {
      os << p.id << " " << p.patient_id << " " << p.average_ct_image_id;
      return os;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    void copy(const Activity & t) {
      id = t.id;
      patient_id = t.patient_id;
      average_ct_image_id = t.average_ct_image_id;
    }
    // --------------------------------------------------

  }; // class Activity
} // namespace syd
// --------------------------------------------------------------------
