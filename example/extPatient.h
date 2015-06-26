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

  /// Example of extended version of a table Patient, we just add an
  /// additional birth_date.
#pragma db object
  class Patient: public syd::Patient {
  public:

    virtual ~Patient() { DD("dest Patient"); }

    /// Additional column 'birth_date'
    std::string birth_date;

    // Set the table name
    SET_TABLE_NAME("Patient")

    /// Required constructor
    Patient();

    /// Overload function to allow to take birth_date into account
    virtual void Set(std::vector<std::string> & arg);

    /// Overload function to print birth_date
    std::string ToString() const;

  }; // end of class
}
// --------------------------------------------------------------------

#endif
