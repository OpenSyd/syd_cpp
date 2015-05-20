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
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonDatabase.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Log (redirect to file)
  Log::SQLFlag() = false;
  Log::LogLevel() = 10;

  std::string pwd;
  syd::GetWorkingDirectory(pwd);
  LOG(1) << "Working dir is " << pwd;

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager * m = syd::DatabaseManager::GetInstance();

  // Create the database
  LOG(1) << "Creating database";
  if (!syd::DirExists("test")) syd::CreateDirectory("test");
  syd::Database * db = m->Create("StandardDatabase", "test.db", "test");

  // Insert some (fake) patients
  {
    syd::Patient p;
    p.Set("toto", 1, 90);
    db->Insert(p);
    syd::Patient t;
  }

  {
    syd::Patient p;
    p.Set("titi", 2, 86);
    db->Insert(p);
  }

  // Insert some injections
  {
    syd::Radionuclide r;
    r.Set("Indium111", 67.313);
    db->Insert(r);
    syd::Patient p = db->QueryOne<syd::Patient>(odb::query<syd::Patient>::study_id == 1);
    syd::Injection i;
    i.Set(p, r, "2024-27-08 18:00", 200.0);
    db->Insert(i);
  }

  {
    syd::Radionuclide r;
    r.Set("Yttrium90", 64.053);
    db->Insert(r);
    syd::Patient p = db->QueryOne<syd::Patient>(odb::query<syd::Patient>::study_id == 1);
    syd::Injection i;
    i.Set(p, r, "2034-27-08 18:00", 180.0);
    db->Insert(i);
  }

  // Create reference is needed
  if (argc > 1) {
    if (std::string(argv[1]) == "create_ref") {
      LOG(0) << "Creating reference output...";
      syd::CopyFile("test.db", "test.ref.db");
    }
    else {
      LOG(WARNING) << "Ignoring parameter " << argv[0];
    }
  }

  // Compare table
  // (echo .dump | sqlite3 test1.db)
  syd::Database * dbref = m->Read<syd::StandardDatabase>("test.ref.db");
  bool b = syd::CompareTable<syd::Patient>(db, dbref);
  if (!b) { LOG(FATAL) << "Table Patient is different between test.db and test.ref.db"; }
  LOG(0) << "Table Patient is ok.";

  b = syd::CompareTable<syd::Injection>(db, dbref);
  if (!b) { LOG(FATAL) << "Table Injection is different between test.db and test.ref.db"; }
  LOG(0) << "Table Patient is ok.";

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
