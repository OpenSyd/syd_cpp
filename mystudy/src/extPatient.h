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

    /// Additional column 'birth_date'
    std::string   birth_date;

    // Set the different table name (must be like in the odb pragma)
    SET_TABLE_NAME("Patient")

    /// Required constructor
    Patient():syd::Patient() { birth_date = "birth_date"; }

    /// Overload function to allow sydInsert birth_date
    virtual void Set(std::vector<std::string> & arg) {
      if (arg.size() < 2) {
        LOG(FATAL) << "To insert patient, please set <name> <study_id> [<weight_in_kg> <dicom_patientid> <birth_date>]";
      }
      syd::Patient::Set(arg);
      if (arg.size() > 4) {
        std::string pdate = arg[4];
        if (!syd::IsDateValid(arg[4])) {
          LOG(FATAL) << "Error while using extPatient::Set, the date is not valid: " << pdate;
        }
        birth_date = pdate;
      }
    }

    /// Overload function to print birth_date
    std::string ToString() const
    {
      std::stringstream ss ;
      ss << syd::Patient::ToString() << " "
         << birth_date;
      return ss.str();
    }


  }; // end of class
}
// --------------------------------------------------------------------

#endif
