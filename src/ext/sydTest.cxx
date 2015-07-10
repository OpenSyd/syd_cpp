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
#include "sfzSynfrizzDatabase.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(sydTest, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname1 = args_info.inputs[0];
  syd::StandardDatabase * db1 = m->Read<syd::StandardDatabase>(dbname1);

  // Get the database 2
  std::string dbname2 = args_info.inputs[1];
  std::string folder = db1->GetDatabaseRelativeFolder();
  DD(folder);
  sfz::SynfrizzDatabase * db2 = static_cast<sfz::SynfrizzDatabase*>(m->Create("SynfrizzDatabase", dbname2, folder));

  std::string l = db1->GetListOfTableNames(); // order count !!
  std::vector<std::string> table_names;
  syd::GetWords(l, table_names);
  DDS(table_names);


  db1->CopyTable("Tag", db2);
  //  db1->CopyTable("Patient", db2);
  // db1->CopyTable("Radionuclide", db2);
  // db1->CopyTable("File", db2);
  // db1->CopyTable("Injection", db2);
  // db1->CopyTable("DicomSerie", db2);
  // db1->CopyTable("DicomFile", db2);

  // for(auto table_name:table_names) {
  //   DD(table_name);
  //   db1->CopyTable(table_name, db2);
  //   // db1->Query(elements);
  //   // DDS(elements);
  //   // db2->Insert(elements);
  // }



  // This is the end, my friend.
}
// --------------------------------------------------------------------
