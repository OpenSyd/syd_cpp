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
  syd::Log::SQLFlag() = false;
  syd::Log::LogLevel() = 1;

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Create the database
  std::string ext_dbname = "test_find.ext.db";
  std::string folder = "test";
  m->Create("ExtendedDatabase", ext_dbname, folder, true);
  ext::ExtendedDatabase * db = m->Read<ext::ExtendedDatabase>(ext_dbname);

  // Insert some records
  ext::Patient::pointer p1;
  db->New(p1);;
  p1->Set(db, "toto", 1, 50,  "XXYYZZ", "2002-08-09 10:00");
  ext::Patient::pointer p2;
  db->New(p2);
  p2->Set(db, "titi", 2, 150, "AXREXYYZZ", "2005-02-01 17:00");
  ext::Patient::pointer p3;
  db->New(p3);
  p3->Set(db, "tutu", 3, 60,  "BXXYRNOEYZZ", "2002-07-17 09:00");
  ext::Patient::pointer p4;
  db->New(p4);
  p4->Set(db, "tata", 4, 80,  "CXREXYYZZ", "2002-08-09 10:00");

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
  db->Set(inj, arg);
  db->Insert(inj);

  db->New(inj);
  arg[0] = "titi"; arg[1] = "Indium111"; arg[2] = "2013-02-14 17:23"; arg[3] = "360.33";
  db->Set(inj, arg);
  db->Insert(inj);

  db->New(inj);
  arg[0] = "titi"; arg[1] = "Indium111"; arg[2] = "2013-04-14 17:23"; arg[3] = "960.33";
  db->Set(inj, arg);
  db->Insert(inj);

  // Create patterns
  std::vector<std::string> patterns;
  patterns.push_back("2002");
  patterns.push_back("RE");
  std::vector<std::string> exclude;
  exclude.push_back("NO");

  {
    syd::Patient::vector patients;
    db->Query(patients);

    //    db->Sort<syd::Patient>(patients); // FIXME

    db->Dump(patients, ""); // output screen
    syd::Patient::vector p;
    db->Grep(p, patients, patterns, exclude);
    db->Dump(p, ""); // output screen

    // create ref
    std::string ref = "test_find.ref1.txt";
    if (argc > 1 and std::string(argv[1]) == "create_ref") {
      std::ofstream of(ref);
      db->Dump(p, "", of);
    }

    // Compare to ref
    std::ifstream is(ref);
    // http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
    std::string str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    std::stringstream oss;
    db->Dump(p, "", oss);
    if (str != oss.str()) {
      sydLOG(syd::FATAL) << "ERROR ref is: " << str << std::endl
                 << " while dump is: " << oss.str();
    }
  }

  {
    syd::Database * db = m->Read(ext_dbname); // generic
    syd::Record::vector records;
    db->Query(records, "Patient");

    syd::Record::vector results;
    db->Grep(results, records, patterns, exclude); // generic find
    db->Dump(results);

    // create ref
    std::string ref = "test_find.ref2.txt";
    if (argc > 1 and std::string(argv[1]) == "create_ref") {
      std::ofstream of(ref);
      db->Dump(results, "", of);
    }
    // Compare to ref
    std::ifstream is(ref);
    std::string str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    std::stringstream oss;
    db->Dump(results, "", oss);
    if (str != oss.str()) {
      sydLOG(syd::FATAL) << "ERROR ref is: " << str << std::endl
                 << " while dump is: " << oss.str();
    }

  }

  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
