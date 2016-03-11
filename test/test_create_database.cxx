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
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "extExtendedDatabase.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  Log::SQLFlag() = false;
  Log::LogLevel() = 5;

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = "test_create_database.db";
  std::string folder = "test";

  // StandardDatabase
  std::cout << "Create StandardDatabase " << dbname << std::endl;
  m->Create("StandardDatabase", dbname, folder, true);

  {
    std::cout << "Open as generic Database" << std::endl;
    syd::Database * db = m->Read(dbname);
    db->Dump(std::cout);
  }

  {
    std::cout << "Open as StandardDatabase" << std::endl;
    syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);
    db->Dump(std::cout);
  }

  // ExtendedDatabase
  std::string ext_dbname = dbname+"-ext.db"; // cannot be the same db name ! dont know why
  // Create
  std::cout << "Create ExtendedDatabase " << ext_dbname << std::endl;
  m->Create("ExtendedDatabase", ext_dbname, folder, true);

  {
    std::cout << "Open as generic Database" << std::endl;
    syd::Database * db = m->Read(ext_dbname);
    db->Dump(std::cout);
  }

  {
    std::cout << "Open as StandardDatabase" << std::endl;
    syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(ext_dbname);
    db->Dump(std::cout);
  }

  {
    std::cout << "Open as ExtendedDatabase" << std::endl;
    ext::ExtendedDatabase * db = m->Read<ext::ExtendedDatabase>(ext_dbname);
    db->Dump(std::cout);
  }


  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
