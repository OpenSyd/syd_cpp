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
#include "sydTableOfRecords.h"
#include "sydPrintTable.h"
#include "sydField.h"

//#include "sydTestTemp9.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydTest, 1);

  // Load plugin and db
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);
  // -----------------------------------------------------------------

  syd::Record::vector records;
  db->Query(records, "Image");
  DDS(records);
  auto r = records[0];
  DD(r);

  //
  /*
  {
    auto f = [](syd::Image::pointer p) -> std::string & { return p->acquisition_date; };
    auto rcast = BuildRefCastFunction(f);
    auto s = BuildToStringFunction<syd::Image, RefCastFunction, std::string>(rcast);
    DD(s(r));
  }
  {
    auto f = [](syd::Image::pointer p) -> std::string { return p->acquisition_date; };
    auto vcast = BuildValueCastFunction(f);
    auto s = BuildToStringFunction<syd::Image, ValueCastFunction, std::string>(vcast);
    DD(s(r));
  }
  {
    auto f = [](syd::Image::pointer p) { return p->patient; };
    auto vcast = BuildValueCastFunction(f);
    auto s = BuildToStringFunction<syd::Image, ValueCastFunction, syd::Patient>(vcast);
    DD(s(r));
  }
  */


  // -----------------------------------------------------------------
  DD("end");
  // This is the end, my friend.
}
// --------------------------------------------------------------------
