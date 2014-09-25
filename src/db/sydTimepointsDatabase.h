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

#ifndef SYDTIMEPOINTSDATABASE_H
#define SYDTIMEPOINTSDATABASE_H

// syd
#include "sydDatabase.h"
#include "Timepoint-odb.hxx"
#include "sydClinicDatabase.h"

// Manage a list of Timepoint. Need a pointer to a ClinicDatabase
// because each Timepoint is linked with a Serie and a Patient from
// this ClinicDatabase;
// --------------------------------------------------------------------
namespace syd {

  class TimepointsDatabase: public Database {

  public:
    TimepointsDatabase(std::string name);
    ~TimepointsDatabase() {}

    std::string GetFullPath(Patient patient);
    std::string GetFullPathSPECT(Timepoint timepoint);
    std::string GetFullPathCT(Timepoint timepoint);
    std::string Print(Timepoint t);
    std::string Print(Patient p);
    void UpdateAllTimepointNumbers(IdType patient_id);
    void set_clinic_database(ClinicDatabase * d) { cdb_ = d; }

  protected:
    ClinicDatabase * cdb_;

  }; // end class
} // end namespace
// --------------------------------------------------------------------

#endif
