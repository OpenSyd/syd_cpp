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
#include "sydInfo_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydCommonGengetopt.h"

// Init syd
SYD_STATIC_INIT


// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydInfo, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();

  // Get the database
  syd::Database * db = syd::DatabaseManager::GetInstance()->Read(args_info.db_arg);

  // define output (could be changeed later)
  auto & os = std::cout;

  // Retrive versions
  auto odb_db = db->GetODB_DB();
  odb::schema_version file_version (odb_db->schema_version(db->GetDatabaseSchema()));
  odb::schema_version current_version
    (odb::schema_catalog::current_version (*odb_db, db->GetDatabaseSchema()));
  odb::schema_version base_version
    (odb::schema_catalog::base_version (*odb_db, db->GetDatabaseSchema()));

  // Basics informations
  os << "Database file  : " << db->GetFilename() << std::endl;
  os << "Database schema: " << db->GetDatabaseSchema() << " " ;
  if (current_version == file_version) os << syd::GetVersionAsString(current_version) << std::endl;
  else { // never here because Read do the migration
    os << warningColor << " file version is " << syd::GetVersionAsString(file_version)
       << " while syd version is " << syd::GetVersionAsString(current_version) << std::endl;
  }
  os << "Database folder: " << db->GetDatabaseRelativeFolder();
  if (!fs::exists(db->GetDatabaseAbsoluteFolder()))
    os << warningColor << " -> does not exist ("
       << db->GetDatabaseAbsoluteFolder() << ")" << resetColor;
  os << std::endl;
  auto map = db->GetMapOfTables();
  if (map.size() > 1) os << map.size() << " tables" << std::endl;
  else os << map.size() << " table" << std::endl;
  for(auto i=map.begin(); i != map.end(); i++) {
    int n = db->GetNumberOfElements(i->first);
    os << "Table: " << std::setw(15) << i->first << " " <<  std::setw(10) << n;
    if (n>1) os << " elements" << std::endl;
    else os << " element" << std::endl;
  }

  // Info about table
  auto dd = db->GetDatabaseDescription();
  for(auto i=0; i<args_info.inputs_num; i++) {
    auto table_name = args_info.inputs[i];
    syd::TableDescription * dt;
    bool b = dd->FindTableDescription(table_name, &dt);
    if (!b) {
      LOG(FATAL) << "Cannot find the table " << table_name;
    }
    dt->Print(os);
  }

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
