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

#ifndef SYDCLINICALTRIALDATABASE_H
#define SYDCLINICALTRIALDATABASE_H

// syd
#include "sydDatabase.h"
#include "Patient-odb.hxx"
#include "Study-odb.hxx"
#include "Serie-odb.hxx"

// inherit from syd::Database

// --------------------------------------------------------------------
namespace syd {

  class ClinicalTrialDatabase: public Database {

  public:
    ClinicalTrialDatabase():Database() {}
    virtual void OpenDatabase();

    void AddPatient(std::string name, Patient & patient);
    void AddStudy(const Patient & patient, std::string uid, std::string date, Study & study);
    void AddSerie(const Study & study, std::string description, std::string uid, Serie & s);

    void CheckPatient(Patient & patient);
    void CheckSerie(Serie & serie);
    void CheckStudy(Study & study);

    std::string GetFullPath(Patient & patient);
    std::string GetFullPath(Study & study);
    std::string GetFullPath(Serie & serie);


  }; // end class
} // end namespace
// --------------------------------------------------------------------

#endif
