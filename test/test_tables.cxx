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
  std::string dbname = "data/test-work.db";
  std::string dbname_copy = "data/test-work-copy.db";
  std::string folder = "test-data";
  std::string ref_dbname1 = "data/test-tables-ref1.db";
  std::string ref_folder = "test-tables-ref-data";
  std::string ref_dbname2 = "data/test-tables-ref2.db";
  LOG(1) << "Creating database " << dbname;
  syd::Database * db = m->Create("StandardDatabase", dbname, folder);

  // Make a copy for the end
  db->CopyDatabaseTo(dbname_copy, folder);

  // Test CRUD - Create Read Update Delete

  // Part 1 insert
  LOG(1) << "---------------- Insert";
  LOG(1) << "Insert Patient";
  syd::Patient p;
  p.Set("toto", 1, 90);
  p.dicom_patientid = "XXAZER1234";
  db->Insert(p);

  LOG(1) << "Insert Radionuclide";
  syd::Radionuclide r;
  r.Set("Indium111", 67.313);
  db->Insert(r);

  LOG(1) << "Insert Injection";
  syd::Injection i;
  i.Set(p, r, "2014-02-01 12:34", 123.45);
  db->Insert(i);

  LOG(1) << "Insert File";
  syd::File f;
  f.filename = "toto.mhd";
  f.path = "/home/yes/";
  f.md5 = "XXXYY123XX";
  db->Insert(f);

  LOG(1) << "Insert Tag";
  syd::Tag t;
  t.label = "mytag";
  t.description = "This is a description";
  db->Insert(t);

  // Once everything inserted, create reference if needed.
  TestCreateReferenceDB(argc, argv, db, ref_dbname1, ref_folder);

  // Compare
  syd::Database * dbref = m->Read<syd::StandardDatabase>(ref_dbname1);
  syd::TestTableEquality<syd::Patient>(db, dbref);
  syd::TestTableEquality<syd::Injection>(db, dbref);
  syd::TestTableEquality<syd::Radionuclide>(db, dbref);
  syd::TestTableEquality<syd::File>(db, dbref);
  syd::TestTableEquality<syd::Tag>(db, dbref);


  // Then again with update
  LOG(1) << "---------------- Update";

  // Part 1 insert
  LOG(1) << "Update Patient";
  syd::Patient pp = db->QueryOne<syd::Patient>(odb::query<syd::Patient>::study_id == 1);
  pp.Set("titi", 2, 666);
  pp.dicom_patientid = "AZERAZER";
  db->Update(pp);

  LOG(1) << "Update Radionuclide";
  syd::Radionuclide rr = db->QueryOne<syd::Radionuclide>(odb::query<syd::Radionuclide>::name == "Indium111");
  rr.Set("Yttrium90", 67.313);
  db->Update(rr);

  LOG(1) << "Update Injection";
  syd::Injection ii = db->QueryOne<syd::Injection>(odb::query<syd::Injection>::id == i.id);
  ii.Set(p, r, "2010-03-10 13:34", 5123.45);
  db->Update(ii);

  LOG(1) << "Update File";
  syd::File ff = db->QueryOne<syd::File>(odb::query<syd::File>::filename == "toto.mhd");
  ff.filename = "titi.mhd";
  ff.path = "/home/no/";
  ff.md5 = "YYYYY";
  db->Update(ff);

  LOG(1) << "Update Tag";
  syd::Tag tt = db->QueryOne<syd::Tag>(odb::query<syd::Tag>::label == "mytag");
  tt.label = "myModifiedTag";
  tt.description = "This is another description";
  db->Update(tt);

 // Once everything inserted, create reference if needed.
  TestCreateReferenceDB(argc, argv, db, ref_dbname2, ref_folder);

  // Compare
  syd::Database * dbref2 = m->Read<syd::StandardDatabase>(ref_dbname2);
  syd::TestTableEquality<syd::Patient>(db, dbref2);
  syd::TestTableEquality<syd::Injection>(db, dbref2);
  syd::TestTableEquality<syd::Radionuclide>(db, dbref2);
  syd::TestTableEquality<syd::File>(db, dbref2);
  syd::TestTableEquality<syd::Tag>(db, dbref2);

  // Delete
  LOG(1) << "---------------- Delete";
  db->Delete(pp);
  //db->Delete(ii); <-- already by patient
  db->Delete(rr);
  db->Delete(ff);
  db->Delete(tt);

  // Compare with initial
  syd::Database * dbcopy = m->Read<syd::StandardDatabase>(dbname_copy);
  syd::TestTableEquality<syd::Patient>(db, dbcopy);
  syd::TestTableEquality<syd::Injection>(db, dbcopy);
  syd::TestTableEquality<syd::Radionuclide>(db, dbcopy);
  syd::TestTableEquality<syd::File>(db, dbcopy);
  syd::TestTableEquality<syd::Tag>(db, dbcopy);

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
