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

#ifndef SYDSTANDARDDATABASE_H
#define SYDSTANDARDDATABASE_H

// syd
#include "sydDatabase.h"
#include "sydTable.h"

// syd tables
#include "sydPatient-odb.hxx"
#include "sydInjection-odb.hxx"

// --------------------------------------------------------------------
namespace syd {

  /// This database manages a set of 'standard' tables: Patient, Injection etc.
  class StandardDatabase: public Database {

  public:

    virtual ~StandardDatabase() {}

    syd::Patient::pointer FindPatient(const std::string & name_or_study_id);

    //    void Dump(std::ostream & os, const std::vector<std::shared_ptr<syd::Patient>> & patients) const;

    // virtual void Dump2(const syd::Patient::pointer patient, std::ostream & os = std::cout) const;
    // virtual void Dump2(const syd::Record::pointer record, std::ostream & os = std::cout) const;

  protected:
    /// Insert the tables
    virtual void CreateTables();

  }; // class StandardDatabase

  #include "sydStandardDatabase.txx"

} // namespace syd

// --------------------------------------------------------------------

#endif
