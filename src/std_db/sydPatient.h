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
#include "sydTableElement.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db model version(1, 1)

#pragma db object
  /// Store information about a patient (id, study_id, name etc).
  class Patient: public syd::TableElement {
  public:

#pragma db id auto
    /// Main key (automated, unique)
    IdType id;

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
    SET_TABLE_NAME("Patient");
    Patient();
    Patient(std::string pname, IdType studyId, double weight);
    virtual ~Patient();
    Patient(const Patient & other);
    Patient & operator= (const Patient & other);
    virtual void copy(const Patient & t);

    virtual std::string ToString() const;
    virtual void SetValues(std::vector<std::string> & arg);
    void Set(std::string pname, IdType studyId, double weight);

    bool operator==(const Patient & p);
    bool operator!=(const Patient & p) { return !(*this == p); }

    bool CheckIdentity(std::string vdicom_patientid, std::string vdicom_name) const;


    static std::string mname;
  }; // end of class
}
// --------------------------------------------------------------------

#endif
