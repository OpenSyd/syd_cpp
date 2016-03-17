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

  syd::Database * db = m->Open(ext_dbname);

  // Insert elements
  {
    ext::Patient::vector patients;
    for(auto i=0; i<5; i++) {
      ext::Patient::pointer patient;
      db->New(patient);
      patient->Set(args);
      args[0] = "toto_"+args[0];
      args[1] = syd::ToString(22+atoi(args[1].c_str()));
      patients.push_back(patient);
    }
    db->Insert(patients);
  }

  {
    std::cout << "Update single generic with arg" << std::endl;
    syd::Record::pointer p;
    db->QueryOne(p, "Patient", 1);
    std::cout << "Get " << p << std::endl;
    args[0] = "titi";
    p->Set(args);
    db->Update(p);
    std::cout << "Update " << p << std::endl;
  }

  {
    std::cout << "Update single with arg" << std::endl;
    ext::Patient::pointer p;
    db->QueryOne(p, 2);
    std::cout << "Get " << p << std::endl;
    args[0] = "titi2";
    args[1] = syd::ToString(123);
    p->Set(args);
    db->Update(p);

    ext::Patient::pointer pref;
    db->New(pref);
    *pref = *p; // standard copy
    db->QueryOne(p, p->id);
    if (!p->IsEqual(pref)) {
      LOG(FATAL) << "Error update single generic. p is " << p << " and pref " << pref;
    }
    std::cout << "Update " << p << std::endl;
  }

  {
    std::cout << "Update multiple generic with arg" << std::endl;
    std::vector<syd::IdType> ids;
    ids.push_back(4);
    ids.push_back(5);
    syd::Record::vector p;
    db->Query(p, "Patient", ids);
    std::cout << "Get " << p.size() << std::endl;

    for(auto i=0; i<p.size(); i++) {
      ext::Patient::pointer p;
      db->New(p);
      args[0] = "toto_"+args[0];
      args[1] = syd::ToString(22+atoi(args[1].c_str()));
      p->Set(args);
    }
    db->Update(p, "Patient");
    p.clear();
    db->Query(p, "Patient", ids);
    std::cout << "Update " << p.size() << std::endl;
  }


  {
    std::cout << "Update multiple with arg" << std::endl;
    ext::Patient::vector p;
    odb::query<ext::Patient> q = odb::query<ext::Patient>::name.like("%toto%");
    db->Query(p, q);
    std::cout << "Get " << p.size() << std::endl;

    for(auto i=0; i<p.size(); i++) {
      auto pp = p[i];
      args[0] = "tutu_"+args[0];
      args[1] = syd::ToString(22+atoi(args[1].c_str()));
      pp->Set(args);
    }
    db->Update(p);
    p.clear();

    q = odb::query<ext::Patient>::name.like("%tutu%");
    db->Query(p, q);
    std::cout << "Update " << p.size() << std::endl;
  }

  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
