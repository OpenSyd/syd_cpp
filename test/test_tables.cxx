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
  db->Insert(p);
  TableElement q = db->QueryOne<TableElement>(p.id);
  bool b = (p == q);
  if (!b) {
    LOG(FATAL) << "Error table " << TableElement::GetTableName() << " during insert.";
  }
  LOG(1) << "Table " << TableElement::GetTableName() << ", insert: " << p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void testUpdate(syd::Database * db, TableElement & p, TableElement & r)
{
  db->Update(p);
  TableElement q = db->QueryOne<TableElement>(p.id);
  bool b = (p == q and p == r);
  if (!b) {
    LOG(FATAL) << "Error table " << TableElement::GetTableName() << " during update.";
  }
  LOG(1) << "Table " << TableElement::GetTableName() << ", update: " << p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void testDelete(syd::Database * db, TableElement & p)
{
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
  LOG(1) << "Table " << TableElement::GetTableName() << ", delete: " << p;
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
  syd::Patient r = p;
  testUpdate(db, p, r);
  testDelete(db, p);

  // Injection
  syd::Injection i;
  db->Insert(p); // re-insert because deleted
  i.Set(p, "Indium", "2014-02-01 12:34", 123.45);
  testInsert(db, i);
  i.activity_in_MBq = 666;
  syd::Injection j = i;
  testUpdate(db, i, j);
  testDelete(db, i);

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
