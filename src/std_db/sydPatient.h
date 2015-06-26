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
#include "sydRecord.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db model version(1, 1)

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Patient")
  /// Store information about a patient (id, study_id, name etc).
  class Patient: public syd::Record {
  public:

    typedef std::shared_ptr<Patient> pointer;

    friend class odb::access;

    virtual ~Patient() { DD("syd::Patient dest");}

    static pointer New() { return pointer(new Patient); }

#pragma db options("UNIQUE")
    /// Patient name (unique)
    std::string name;

#pragma db options("UNIQUE")
    /// Patient number in the study (unique)
    IdType study_id;

    /// Patient weight_in_kg
    double weight_in_kg;

    /// Patient dicom ID. Not unique because could be unknown.
    std::string dicom_patientid;

    // ------------------------------------------------------------------------
    //    Patient();
    virtual std::string ToString() const;

   protected:
    Patient():Record("") { name = "unset_name"; DD("const sydpatient"); }


  }; // end of class
}
// --------------------------------------------------------------------

#endif
