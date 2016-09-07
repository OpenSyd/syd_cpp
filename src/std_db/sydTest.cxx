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
#include "sydTest_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"
#include "sydTimepointsBuilder.h"
#include "sydRecordHelpers.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydTest, 1);

  // Load plugin


  //  syd::DatabaseManager::GetInstance()->RegisterDatabaseSchema<syd::StandardDatabase>("StandardDatabase");

  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::PluginManager::GetInstance()->Load();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // ------------------------------------------------------------------
  // test
  if (1) {
    // auto images = db->FindImages("2");
    // if (images.size() > 0) {  DD(images[0]); }
    syd::Patient::vector patients;
    db->Query(patients);
    DDS(patients);
  }


  // ------------------------------------------------------------------
  syd::PluginManager::GetInstance()->UnLoad();
  DD("end");
  // This is the end, my friend.
}
// --------------------------------------------------------------------
