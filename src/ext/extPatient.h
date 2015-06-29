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

#pragma db object table("ext::Patient")
  /// Store information about a patient (id, study_id, name etc).
  class Patient: public syd::Patient {
  public:

    typedef std::shared_ptr<ext::Patient> pointer;

    friend class odb::access;

    virtual ~Patient() { }

    static pointer New() { return pointer(new ext::Patient); }

    virtual void Set(const syd::Database * db, const std::vector<std::string> & args);

    virtual void Set(const syd::Database * db, const std::string & pname,
                     const syd::IdType & pstudy_id, const double pweight_in_kg=-1,
                     const std::string pdicom_patientid="unset_dicom_patientid",
                     const std::string pbirth_date="0000-00-00 00:00");

    //
    std::string birth_date;

    // ------------------------------------------------------------------------
    //    Patient();

    virtual std::string ToString() const;
  protected:
    Patient():syd::Patient() { birth_date = "2015"; }

  }; // end of class
}
// --------------------------------------------------------------------

#endif
