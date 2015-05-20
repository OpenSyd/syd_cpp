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
template<class TableElement>
void testInsert(syd::Database * db, TableElement & p)
{
  LOG(1) << "Table " << TableElement::GetTableName() << ", insert: " << p << " ...";
  db->Insert(p);
  TableElement q = db->QueryOne<TableElement>(p.id);
  bool b = (p == q);
  if (!b) {
    LOG(FATAL) << "Error table " << TableElement::GetTableName() << " during insert.";
  }
  LOG(1) << p << "inserted.";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void testUpdate(syd::Database * db, TableElement & p, TableElement & r)
{
  LOG(1) << "Table " << TableElement::GetTableName() << ", update: " << p << " ...";
  db->Update(p);
  TableElement q = db->QueryOne<TableElement>(p.id);
  bool b = (p == q and p == r);
  if (!b) {
    LOG(FATAL) << "Error table " << TableElement::GetTableName() << " during update.";
  }
  LOG(1) << p << " updated.";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void testDelete(syd::Database * db, TableElement & p)
{
  LOG(1) << "Table " << TableElement::GetTableName() << ", delete: " << p << " ...";
  std::vector<TableElement> l;
  db->Query(l);
  int before = l.size();
  db->Delete(p);
  l.clear();
  db->Query(l);
  bool b = (l.size() == before-1);
  if (!b) {
    LOG(FATAL) << "Error table " << TableElement::GetTableName() << " during delete.";
  }
  LOG(1) << p << " deleted.";
}
// --------------------------------------------------------------------


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

  // Test CRUD - Create Read Update Delete

  // Patient
  syd::Patient p;
  p.Set("toto", 1, 90);
  testInsert(db, p);
  p.name = "titi";
  p.weight_in_kg = 123;
  p.study_id = 16;
  p.dicom_patientid = "xxx";
  syd::Patient pp = p;
  testUpdate(db, p, pp);
  testDelete(db, p);
  db->Insert(p); // re-insert patient (because use later)

  // Radionuclide
  syd::Radionuclide r;
  r.Set("Indium111", 67.313);
  testInsert(db, r);
  r.name = "Indium-111";
  syd::Radionuclide rr = r;
  testUpdate(db, r, rr);
  testDelete(db, r);
  db->Insert(r);

  // Injection
  syd::Injection i;
  i.Set(p, r, "2014-02-01 12:34", 123.45);
  testInsert(db, i);
  i.activity_in_MBq = 666;
  syd::Injection j = i;
  testUpdate(db, i, j);
  testDelete(db, i);
  testInsert(db, i);

  // File
  syd::File f;
  f.filename = "toto";
  f.path = "/home/yes/";
  f.md5 = "XXXXX";
  testInsert(db, f);
  f.filename = "bidon";
  syd::File ff = f;
  testUpdate(db, f, ff);
  testDelete(db, f);
  testInsert(db, f);

  // Tag
  syd::Tag t;
  t.label = "mytag";
  t.description = "this is a tag";
  testInsert(db, t);
  t.label = "bidon";
  syd::Tag tt = t;
  testUpdate(db, t, tt);
  testDelete(db, t);

  // DicomSerie
  syd::DicomSerie ds;
  ds.patient = std::make_shared<syd::Patient>(p);
  ds.injection = std::make_shared<syd::Injection>(i);
  ds.acquisition_date = "2214-02-01 12:34";
  ds.reconstruction_date = "1014-02-01 12:34";
  ds.dicom_study_uid = "XXYY";
  ds.dicom_series_uid = "RTRTRTRTRRT";
  ds.dicom_frame_of_reference_uid = "ZERTAERT";
  ds.dicom_modality = "CT";
  ds.dicom_manufacturer = "bidon";
  ds.dicom_description = "this is a description";
  ds.size[0] = 1 ; ds.size[1] = 2 ; ds.size[2] = 3;
  ds.spacing[0] = 1.2 ; ds.spacing[1] = 2.3 ; ds.spacing[2] = 3.2;
  testInsert(db, ds);
  ds.dicom_modality = "NM";
  syd::DicomSerie dds = ds;
  testUpdate(db, ds, dds);
  testDelete(db, ds);
  testInsert(db, ds); // re-insert

  // DicomFile
  syd::DicomFile df;
  df.file = std::make_shared<syd::File>(f);
  df.dicom_serie = std::make_shared<syd::DicomSerie>(ds);
  df.dicom_sop_uid = "AERTARIAZOERIPEOIR";
  df.dicom_instance_number = 12;
  testInsert(db, df);
  df.dicom_sop_uid = "bidon";
  syd::DicomFile ddf = df;
  testUpdate(db, df, ddf);
  testDelete(db, df);

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
