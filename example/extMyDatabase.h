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
#include "sydInjection-odb.hxx"
#include "sydDicomSerie-odb.hxx"
#include "sydDicomFile-odb.hxx"
#include "sydFile-odb.hxx"
#include "sydTag-odb.hxx"
#include "sydTimepoint-odb.hxx"

// --------------------------------------------------------------------
namespace ext {

  /// Example of extended version of a StandardDatabase that
  /// potentially include standard tables, extended tables and new
  /// tables.
  class MyDatabase: public syd::StandardDatabase {
  public:

    /// Overload CreateTables to insert the new tables (required)
    virtual void CreateTables();

  }; // class MyDatabase
} // namespace ext

// --------------------------------------------------------------------

#endif
