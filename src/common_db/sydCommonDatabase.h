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
  ===========================================================================**/

#ifndef SYDCOMMONDATABASE_H
#define SYDCOMMONDATABASE_H

// syd
#include "sydDatabase.h"
#include "sydDatabaseInformation-odb.hxx"
#include "sydRecord-odb.hxx"
#include "sydFile-odb.hxx"
#include "sydRecordHistory-odb.hxx"
#include "sydRecordWithHistory-odb.hxx"
#include "sydTag-odb.hxx"
#include "sydRecordWithTags-odb.hxx"

#include "sydTagTraits.h"
#include "sydFileTraits.h"
#include "sydRecordHistoryTraits.h"

// --------------------------------------------------------------------
namespace syd {

  class CommonDatabase: public Database {
  public:
    CommonDatabase();
    virtual ~CommonDatabase();

  protected:
    /// Insert the tables
    virtual void CreateTables();
  }; // end class

} // end namespace
// --------------------------------------------------------------------
#endif
