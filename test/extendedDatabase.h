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

#ifndef EXTSTANDARDDATABASE_H
#define EXTSTANDARDDATABASE_H

// syd
#include "sydStandardDatabase.h"

// ext
#include "extPatient-odb.hxx"

// --------------------------------------------------------------------
namespace ext {

  /// Example of extended version of a StandardDatabase
  class extendedDatabase: public syd::StandardDatabase {
  public:

    /// Required CreateTables overloading
    virtual void CreateTables() {
      LOG(0) << "I am a extended DB";
      AddTable<ext::Patient>();
      //AddTable<syd::Injection>("Toto");
    }

  }; // class extendedDatabase
} // namespace ext

// --------------------------------------------------------------------

#endif
