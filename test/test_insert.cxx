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
  Log::LogLevel() = 1;

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = "test_insert.db";
  std::string ext_dbname = "test_insert.ext.db";
  std::string folder = "test";

  std::vector<std::string> args;
  args.push_back("toto");
  args.push_back("1");
  args.push_back("50");
  args.push_back("XXXXX");
  args.push_back("2002-08-09 10:00");

  //----------------------------------------------------------------
  {  // StandardDatabase
    std::cout << "Create db " << std::endl;
    m->Create("StandardDatabase", dbname, folder, true);
    m->Create("ExtendedDatabase", ext_dbname, folder, true);
  }

  //----------------------------------------------------------------
  {
    std::cout << "Open std as generic Database" << std::endl;
    syd::Database * db = m->Read(dbname);
    db->Dump();
    auto patient = db->New("Patient");
    db->Set(patient, args);
    std::cout << "Before insertion " << patient << std::endl;
    db->Insert(patient);
    std::cout << "After insertion " << patient << std::endl;
    if (patient->id != 1) {
      LOG(FATAL) << "Error while inserting generic patient in std db" << patient;
    }
  }

  //----------------------------------------------------------------
  {
    std::cout << "Open ext as generic Database" << std::endl;
    syd::Database * db = m->Read<syd::Database>(ext_dbname);
    auto patient = db->New("Patient");
    db->Set(patient, args);
    std::cout << "Before insertion " << patient << std::endl;
    db->Insert(patient);
    std::cout << "After insertion " << patient << std::endl;
    if (patient->id != 1) {
      LOG(FATAL) << "Error while inserting generic patient in ext db" << patient;
    }
  }

  //----------------------------------------------------------------
  {
    std::cout << "Open std as StandardDatabase" << std::endl;
    syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);
    auto patient = syd::Patient::New();
    args[0] = "titi";
    args[1] = "2";
    db->Set(patient, args);
    std::cout << "Before insertion " << patient << std::endl;
    db->Insert(patient);
    std::cout << "After insertion " << patient << std::endl;
    if (patient->id != 1 and patient->name != "titi" and patient->study_id != 2) {
      LOG(FATAL) << "Error while inserting std patient in std db" << patient;
    }

  }

  //----------------------------------------------------------------
  {
    std::cout << "Open ext as StandardDatabase" << std::endl;
    syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(ext_dbname);
    auto patient = syd::Patient::New();
    args[0] = "titi";
    args[1] = "2";
    db->Set(patient, args); // in that case the birth_date is not initialized
    std::cout << "Before insertion " << patient << std::endl;
    db->Insert(patient);
    std::cout << "After insertion " << patient << std::endl;
    if (patient->id != 1 and patient->name != "titi" and patient->study_id != 2) {
      LOG(FATAL) << "Error while inserting std patient in ext db" << patient;
    }
  }

  //----------------------------------------------------------------
  {
    std::cout << "Open ext as ExtendedDatabase" << std::endl;
    ext::ExtendedDatabase * db = m->Read<ext::ExtendedDatabase>(ext_dbname);
    auto patient = ext::Patient::New();
    args[0] = "tutu";
    args[1] = "3";
    db->Set(patient, args);
    std::cout << "Before insertion " << patient << std::endl;
    db->Insert(patient);
    std::cout << "After insertion " << patient << std::endl;
    if (patient->id != 1 and patient->name != "tutu" and patient->study_id != 3) {
      LOG(FATAL) << "Error while inserting ext patient in ext db" << patient;
    }
  }

  //----------------------------------------------------------------
  {
    ext::ExtendedDatabase * db = m->Read<ext::ExtendedDatabase>(ext_dbname);
    ext::Patient::vector patients;
    auto p = ext::Patient::New();
    p->Set(db, "atoto", 10, 50,  "XXYYZZ", "2002-08-09 10:00");
    patients.push_back(p);
    p = ext::Patient::New();
    p->Set(db, "atiti", 20, 150, "AXXYYZZ", "2005-02-01 17:00");
    patients.push_back(p);
    p = ext::Patient::New();
    p->Set(db, "atutu", 30, 60,  "BXXYYZZ", "2009-07-17 09:00");
    patients.push_back(p);
    p = ext::Patient::New();
    p->Set(db, "atata", 40, 80,  "CXXYYZZ", "2002-08-09 10:00");
    patients.push_back(p);
    db->Insert<ext::Patient>(patients);

    ext::Patient::vector vp;
    db->Query(vp);
    if (vp.size() != 6)  {
      LOG(FATAL) << "Error while inserting mulitple ext patient in ext db";
    }
  }

  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
