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
  std::string ext_dbname = "test_insert.ext.db";
  std::string folder = "test";
  m->Create("ExtendedDatabase", ext_dbname, folder, true);

  // Read the database
  ext::ExtendedDatabase * db = m->Read<ext::ExtendedDatabase>(ext_dbname);

  /// Insert element
  auto p1 = ext::Patient::New();
  p1->Set(db, "toto", 1, 50,  "XXYYZZ", "2002-08-09 10:00");
  auto p2 = ext::Patient::New();
  p2->Set(db, "titi", 2, 150, "AXXYYZZ", "2005-02-01 17:00");
  auto p3 = ext::Patient::New();
  p3->Set(db, "tutu", 3, 60,  "BXXYYZZ", "2009-07-17 09:00");
  auto p4 = ext::Patient::New();
  p4->Set(db, "tata", 4, 80,  "CXXYYZZ", "2002-08-09 10:00");

  db->Insert(p1);
  db->Insert(p2);
  db->Insert(p3);
  db->Insert(p4);

  //----------------------------------------------------------------
  // QueryOne
  {
    syd::Record::pointer r;
    db->QueryOne(r, "Patient", 1);
    std::cout << "QueryOne record: " << r << std::endl;
    if (!r->IsEqual(p1)) {
      LOG(FATAL) << "Error while comparing r and p1: " << r << " " << p1;
    }

    syd::Patient::pointer p;
    db->QueryOne(p, 1);
    std::cout << "QueryOne syd::Patient " << p << std::endl;
    if (!p->IsEqual(p1)) {
      LOG(FATAL) << "Error while comparing p and p1: " << p << " " << p1;
    }

    ext::Patient::pointer q;
    db->QueryOne(q, 1);
    std::cout << "QueryOne ext::Patient " << q << std::endl;
    if (!q->IsEqual(p1)) {
      LOG(FATAL) << "Error while comparing p and p1: " << q << " " << p1;
    }
    //DD(&q); DD(&p1); -> not the same object, but similar in the db

    try {
      db->QueryOne(q, 10);
    } catch (std::exception & e) {
      std::cout << "QueryOne : Ok, exception caught, cannot find patient 10. " << std::endl;
    }

    std::cout << "Query by name 'tata': " << p << std::endl;
    typedef odb::query<ext::Patient> Q;
    Q query(Q::name == "tata");
    ext::Patient::pointer patient;
    db->QueryOne(patient, query);
    if (!patient->IsEqual(p4)) {
      LOG(FATAL) << "Error while comparing p and p4: " << patient << " <--> " << p4;
    }
  }

  // Query multiple

  {
    std::cout << std::endl;
    ext::Patient::vector patients;
    typedef odb::query<ext::Patient> Q;
    Q query(Q::name != "titi");
    db->Query(patients, query);
    if (!p1->IsEqual(patients[0]) or
        !p3->IsEqual(patients[1]) or
        !p4->IsEqual(patients[2])) {
      LOG(FATAL) << "Error while Query n patients";
    }
    std::cout << "Query n patients: " << patients.size() << std::endl;
  }

  {
    ext::Patient::vector patients;
    db->Query(patients);
    if (!p1->IsEqual(patients[0]) or
        !p2->IsEqual(patients[1]) or
        !p3->IsEqual(patients[2]) or
        !p4->IsEqual(patients[3])) {
      LOG(FATAL) << "Error while Query all patients";
    }
    std::cout << "Query all n patients: " << patients.size() << std::endl;
  }

  // List of id to retrieve
  std::vector<syd::IdType> ids;
  ids.push_back(1);
  ids.push_back(3);
  {
    ext::Patient::vector patients;
    db->Query(patients, ids);
    if (!p1->IsEqual(patients[0]) or
        !p3->IsEqual(patients[1])) {
      LOG(FATAL) << "Error while Query patients by ids";
    }
    std::cout << "Query patients by ids: " << patients.size() << std::endl;
  }
  {
    syd::Record::vector records;
    db->Query(records, "Patient", ids);
    if (!records[0]->IsEqual(p1) or
        !records[1]->IsEqual(p3)) {
      LOG(FATAL) << "Error while Query generic patients by ids";
    }
    std::cout << "Query generic by ids: " << records.size() << std::endl;
  }

  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
