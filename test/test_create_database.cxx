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
#include "sydTestUtils.h"
#include "sydStandardDatabase.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  syd::TestInitialisation();
  syd::DatabaseManager * m = syd::DatabaseManager::GetInstance();

  // Create the database
  std::string dbname = "test-work.db";
  std::string folder = "test-data";
  std::string ref_dbname = "test-ref.db";
  std::string ref_folder = "test-ref-data";
  LOG(1) << "Creating database " << dbname;
  syd::Database * db = m->Create("StandardDatabase", dbname, folder);

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
  TestCreateReferenceDB(argc, argv, db, ref_dbname, ref_folder);

  // Compare table
  syd::Database * dbref = m->Read<syd::StandardDatabase>(ref_dbname);
  syd::TestTableEquality<syd::Patient>(db, dbref);
  syd::TestTableEquality<syd::Injection>(db, dbref);

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------