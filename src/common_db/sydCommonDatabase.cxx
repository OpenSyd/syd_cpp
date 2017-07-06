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

// syd
#include "sydCommonDatabase.h"
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
syd::CommonDatabase::CommonDatabase():syd::Database()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CommonDatabase::~CommonDatabase()
{
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
void syd::CommonDatabase::CreateTables()
{
  //  syd::Database::CreateTables();
  AddTable<syd::Tag>();
  AddTable<syd::File>();
  AddTable<syd::RecordHistory>(); // needed for traits
}
// --------------------------------------------------------------------

