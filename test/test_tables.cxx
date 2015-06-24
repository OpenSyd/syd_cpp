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
  std::string init_dbname = "data/test.db";
  std::string work_dbname = "data/test-work.db";
  std::string work_folder = "test-work-data";
  std::string ref_dbname1 = "data/test-tables-ref1.db";
  std::string ref_folder = "test-tables-ref-data";
  std::string ref_dbname2 = "data/test-tables-ref2.db";
  LOG(1) << "Creating database " << work_dbname;
  syd::StandardDatabase * db_init = m->Read<syd::StandardDatabase>(init_dbname);
  // Make a copy
  db_init->CopyDatabaseTo(work_dbname, work_folder);
  // Open the copy
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(work_dbname);

  // Test CRUD - Create Read Update Delete

  // Part 1 insert
  LOG(1) << "---------------- Insert";
  LOG(1) << "Insert Patient";
  syd::Patient p;
  p.Set("toto", 78, 90);
  p.dicom_patientid = "XXAZER1234";
  db->Insert(p);

  LOG(1) << "Insert Radionuclide";
  syd::Radionuclide r;
  r.Set("Indium666", 67.313);
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
  syd::Patient pp = db->QueryOne<syd::Patient>(odb::query<syd::Patient>::study_id == 78);
  pp.Set("titi", 987, 666);
  pp.dicom_patientid = "AZERAZER";
  db->Update(pp);

  LOG(1) << "Update Radionuclide";
  syd::Radionuclide rr = db->QueryOne<syd::Radionuclide>(odb::query<syd::Radionuclide>::name == "Indium666");
  rr.Set("Yttrium900", 67.313);
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
  //syd::Database * db_init = m->Read<syd::StandardDatabase>(dbname_copy);
  syd::TestTableEquality<syd::Patient>(db, db_init);
  syd::TestTableEquality<syd::Injection>(db, db_init);
  syd::TestTableEquality<syd::Radionuclide>(db, db_init);
  syd::TestTableEquality<syd::File>(db, db_init);
  syd::TestTableEquality<syd::Tag>(db, db_init);

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
