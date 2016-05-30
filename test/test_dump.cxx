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

  // Create the database
  std::string ext_dbname = "test_dump.ext.db";
  std::string folder = "test";
  m->Create("ExtendedDatabase", ext_dbname, folder, true);
  ext::ExtendedDatabase * db = m->Open<ext::ExtendedDatabase>(ext_dbname);

  // Insert some records
  ext::Patient::pointer p1;
  db->New(p1);
  p1->Set("toto", 1, 50,  "XXYYZZ", "2002-08-09 10:00");
  ext::Patient::pointer p2;
  db->New(p2);
  p2->Set("titi", 2, 150, "AXXYYZZ", "2005-02-01 17:00");
  ext::Patient::pointer p3;
  db->New(p3);
  p3->Set("tutu", 3, 60,  "BXXYYZZ", "2009-07-17 09:00");
  ext::Patient::pointer p4;
  db->New(p4);
  p4->Set("tata", 4, 80,  "CXXYYZZ", "2002-08-09 10:00");

  db->Insert(p1);
  db->Insert(p2);
  db->Insert(p3);
  db->Insert(p4);

  syd::Radionuclide::pointer r;
  db->New(r);
  r->name = "Indium111";
  r->half_life_in_hours = 67.313;
  db->Insert(r);

  syd::Injection::pointer inj;
  std::vector<std::string> arg(4);

  db->New(inj);
  arg[0] = "toto"; arg[1] = "Indium111"; arg[2] = "2003-12-04 12:23"; arg[3] = "160.21";
  inj->Set(arg);
  db->Insert(inj);

  db->New(inj);
  arg[0] = "titi"; arg[1] = "Indium111"; arg[2] = "2013-02-14 17:23"; arg[3] = "360.33";
  inj->Set(arg);
  db->Insert(inj);

  db->New(inj);
  arg[0] = "titi"; arg[1] = "Indium111"; arg[2] = "2013-04-14 17:23"; arg[3] = "960.33";
  inj->Set(arg);
  db->Insert(inj);

  // Dump knowing the record type
  {
    ext::Patient::vector patients;
    db->Query(patients); // all patients
    //    std::ofstream of(ext_dbname+"_out.txt");
    db->Dump(patients); // default format
    db->Dump(patients, "help"); // help format
    db->Dump(patients, "injection"); // format with injections

    syd::Injection::vector injections;
    db->Query(injections);
    db->Dump(injections);

    // Compare to reference output ?
  }

  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
