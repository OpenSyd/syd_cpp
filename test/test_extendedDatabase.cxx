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
#include "test_extendedDatabase_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "extExtendedDatabase.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(test_extendedDatabase, 1);

  Log::SQLFlag() = false;
  Log::LogLevel() = 1;

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];

  // StandardDatabase
  {
    // Create
    std::string b = dbname+".backup";
    std::rename(dbname.c_str(), b.c_str());
    std::string folder = "test";
    LOG(1) << "Create StandardDatabase " << dbname;
    m->Create("StandardDatabase", dbname, folder);
  }

  {
    // Open as StandardDatabase
    syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);
    db->Dump(std::cout);

    // Create a new patient
    // syd::Patient * a = new syd::Patient; // MUST NE IMPOSSIBLE

    auto patient = db->NewPatient();
    std::cout << "Before insertion " << patient << std::endl;
    db->Insert(patient);
    std::cout << "After insertion " << patient << std::endl;
    if (patient->id != 1) {
      LOG(FATAL) << "Error during Patient insertion";
    }
    delete db;
  }


  // ExtendedDatabase
  {
    dbname = dbname+"-ext.db"; // cannot be the same db name ! dont know why
    // Create
    std::string b = dbname+".backup";
    std::rename(dbname.c_str(), b.c_str());
    std::string folder = "test";
    LOG(1) << "Create ExtendedDatabase " << dbname;
    m->Create("ExtendedDatabase", dbname, folder);
  }

  {
    // Open as StandardDatabase
    ext::ExtendedDatabase * db = m->Read<ext::ExtendedDatabase>(dbname);
    db->Dump(std::cout);

    // Create a new patient
    // ext::Patient * a = new ext::Patient; // MUST NE IMPOSSIBLE

    auto patient = db->NewPatient();
    std::cout << "Before insertion " << patient << std::endl;
    db->Insert(patient);
    std::cout << "After insertion " << patient << std::endl;
    if (patient->id != 1) {
      LOG(FATAL) << "Error during Patient insertion";
    }
  }

  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
