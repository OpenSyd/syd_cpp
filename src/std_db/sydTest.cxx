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
#include "sydPrintTable2.h"

#include <boost/variant.hpp>

//#include "sydTestTemp3.h"
// #include "sydTestTemp4.h"
#include "sydTestTemp5.h"

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


  struct A {
    int a;
    std::string b;
  };
  boost::variant<int, std::string> a;
  a = 12;
  DD(a);
  a = "toto";
  DD(a);
  // DD(a.size()); // does not work obviously

  auto s = boost::get<std::string>(&a);
  if (s) {
    DD("this is a string");
    DD(*s);
  }

  exit(0);

  // -----------------------------------------------------------------
  syd::Image::vector images;
  db->Query(images);
  DD(images.size());
  auto table_name = images[0]->GetTableName();
  auto columns = "raw";

  syd::Record::vector records;
  for(auto &im:images) records.push_back(im);

  // Step1
  //auto columns_info = TableBuildColumns(table_name, columns);

  // Step2
  syd::PrintTable2 table;
  auto fields = db->GetFields(table_name, "raw");
  table.Build(records, fields);
  table.Print(std::cout);
  //  table.Build(records, "patient.id");
  // table.SetHeaderFlag(true); // options
  // Step3
  //table.Print(std::cout);


  // -----------------------------------------------------------------
  if (0) {

    syd::Image::vector images;
    db->Query(images);
    DD(images.size());
    DD(images[0]->GetTableName());
    for(auto i:images) std::cout << i->id << " "; std::cout << std::endl;
    db->Sort<syd::Image>(images);
    db->Sort(images);
    for(auto i:images) std::cout << i->id << " "; std::cout << std::endl;

    syd::Record::vector records;
    db->Query(records, "Image");
    DD(records.size());
    for(auto i:records) std::cout << i->id << " "; std::cout << std::endl;
    db->Sort(records, "Image", "date");
    for(auto i:records) std::cout << i->id << " "; std::cout << std::endl;

  }
  // -----------------------------------------------------------------


  DD("end");
  // This is the end, my friend.
}
// --------------------------------------------------------------------
