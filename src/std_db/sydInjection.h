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

#ifndef SYDINJECTION_H
#define SYDINJECTION_H

// syd
#include "sydPatient.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object
  /// Store information about a radionuclide injection (date, etc)
  /**
      Element of table stored in a db. Contains information about a
      patient injection of a radionuclide (quantity, date, type of
      radionuclide)
  */
  class Injection : public TableElement {
  public:

#pragma db id auto
    IdType        id;

    /// Foreign key, it must exist in the Patient table
    std::shared_ptr<Patient> patient;

    std::string   radionuclide;
    std::string   date;
    double        activity_in_MBq;

    // ------------------------------------------------------------------------
    SET_TABLE_NAME("Injection")
    Injection();
    Injection(Patient & p, const std::string & pradionuclide, const std::string & pdate, double activity);
    ~Injection();
    Injection(const Injection & other);
    Injection & operator= (const Injection & other);
    void copy(const Injection & t);

    virtual std::string ToString() const;
    void Set(Patient & p, const std::string & pradionuclide,
             const std::string & pdate, double activity);

    bool operator==(const Injection & p);
    bool operator!=(const Injection & p) { return !(*this == p); }

  }; // end of class
} // end of namespace
// --------------------------------------------------------------------

#endif
