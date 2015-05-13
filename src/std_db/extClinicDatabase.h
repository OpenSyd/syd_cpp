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

#ifndef EXTCLINICDATABASE_H
#define EXTCLINICDATABASE_H

// syd
#include "sydClinicDatabase.h"

// ext
#include "extPatient-odb.hxx"

// --------------------------------------------------------------------
namespace ext {

  /// Example of extended version of a ClinicDatabase
  class ClinicDatabase: public syd::ClinicDatabase {
  public:
    /// Required constructor overloading
    //    ClinicDatabase(std::string file):syd::ClinicDatabase(file) { }

    /// Required CreateTables overloading
    virtual void CreateTables() {
      LOG(0) << "I am a extclinicdatabase_h";
      AddTable<ext::Patient>();
      //AddTable<syd::Injection>("Toto");
    }

  }; // class ext::ClinicDatabase
} // namespace ext

// --------------------------------------------------------------------

#endif
