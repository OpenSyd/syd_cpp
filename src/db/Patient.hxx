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

#ifndef SYDPATIENT_H
#define SYDPATIENT_H

// syd
#include "sydCommon.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object
  class Patient  {
  public:

#pragma db id auto
    IdType        id;
    std::string   name;
    IdType        synfrizz_id;
    double        weight_in_kg;
    std::string   path;
    bool          was_treated;
    std::string   injection_date;
    double        injected_activity_in_MBq;

    // --------------------------------------------------
    Patient() {}
    ~Patient() {}
    Patient(const Patient & other) { copy(other); }
    // --------------------------------------------------


    // --------------------------------------------------
    Patient & operator= (const Patient & other) {
      if (this != &other) { copy(other); }
      return *this;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const Patient & p) {
      os << p.synfrizz_id << " " << p.name;
      return os;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    void copy(const Patient & t) {
      id = t.id;
      name = t.name;
      synfrizz_id = t.synfrizz_id;
      weight_in_kg = t.weight_in_kg;
      path = t.path;
      was_treated = t.was_treated;
      injection_date = t.injection_date;
      injected_activity_in_MBq = t.injected_activity_in_MBq;
    }
    // --------------------------------------------------

  }; // end of class

}
// --------------------------------------------------------------------

#endif
