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

#ifndef EXTPATIENT_H
#define EXTPATIENT_H

// syd
#include "sydPatient.h"

// --------------------------------------------------------------------
namespace ext {

#pragma db model version(1, 1)

#pragma db object
  /// Example of extended version of a table Patient
  class Patient: public syd::Patient {
  public:

    /// Additional column 'birth_date'
    std::string   birth_date;

    /// Required copy overloading to consider the new column
    // virtual void copy(const ext::Patient & t) {
    //   syd::Patient::copy(t);
    //   birth_date = t.birth_date;
    // }

    /// --------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const Patient & p)  {
      os << "(extPatient) " << p.id << p.study_id << " " << p.name << " " << p.birth_date;
      return os;
    }
    // --------------------------------------------------

  }; // end of class
}
// --------------------------------------------------------------------

#endif
