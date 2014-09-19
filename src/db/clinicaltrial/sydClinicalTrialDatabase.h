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
#include "Serie-odb.hxx"
#include "sydDicomCommon.h"

// inherit from syd::Database
// --------------------------------------------------------------------
namespace syd {

  class ClinicalTrialDatabase: public Database {

  public:
    ClinicalTrialDatabase():Database() {}
    virtual void OpenDatabase();

    void UpdateSerie(Serie & serie);

    void set_check_file_content_level(int l) { check_file_content_level_ = l; }
    void CheckPatient(const Patient & patient);
    void CheckSerie(const Serie & serie);
    void CheckSerie_CT(const Serie & serie);
    void CheckSerie_NM(const Serie & serie);

    std::string GetFullPath(const Patient & patient);
    std::string GetFullPath(const Serie & serie);

    odb::query<Serie> GetSeriesQueryFromPatterns(std::vector<std::string> patterns);
    void AndSeriesQueryFromPattern(odb::query<Serie> & q, std::string pattern);

  protected:
    int check_file_content_level_;

  }; // end class
} // end namespace
// --------------------------------------------------------------------

#endif
