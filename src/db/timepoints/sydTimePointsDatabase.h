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
#include "TimePoint-odb.hxx"
#include "sydClinicalTrialDatabase.h"

// inherit from syd::Database
// --------------------------------------------------------------------
namespace syd {

  class TimePointsDatabase: public Database {

  public:
    TimePointsDatabase():Database() {}
    ~TimePointsDatabase() {}

    std::string GetFullPath(Patient patient);
    std::string GetFullPathSPECT(TimePoint timepoint);
    std::string GetFullPathCT(TimePoint timepoint);

    void UpdateAllTimePointNumbers(IdType patient_id);

    void set_clinicaltrial_database(ClinicalTrialDatabase * d) { cdb_ = d; }

  protected:
    ClinicalTrialDatabase * cdb_;


  }; // end class
} // end namespace
// --------------------------------------------------------------------

#endif
