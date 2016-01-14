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
#include "sydTest_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"
//#include "extExtendedDatabase.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydTest, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // get db description
  // print info
  /*
  auto d = db->GetDatabaseDescription();
  DD(d.GetDatabaseSchemaName());
  auto tables = db->GetTables();
  for(auto t:tables) {
    DD(t.GetName());   // Patient
    auto fields = db->GetFields();
    for(auto f:fields) {
      DD(f.GetName());    // Image size
      //      DD(f.GetSQLName()); // Image_size
    }
  }
  */

  // auto map = odb::access::object_traits_impl< ::syd::Patient, odb::id_sqlite >::map;
  // auto type_map = map->type_map_;

  DD("here");
  db->InitDatabaseDescription();
  auto desc = db->GetDatabaseDescription();
  DD(*desc);

  syd::Record::pointer r;
  syd::IdType id = 21;
  db->QueryOne(r, "Patient", id);
  DD(r);
  //  db->Update(r, "name", "toto");

  auto tdesc = desc->GetTableDescription("Patient");
  DD(tdesc);
  //  auto field = tdesc->GetField("dicom_patientid");
  auto field = tdesc->GetField("birth_date");
  DD(field);

  std::string table_sql_name = field->GetSQLTableName();
  std::string field_sql_name = field->GetName();
  std::string value = syd::Now();

  std::ostringstream sql;
  sql << "UPDATE \"" << table_sql_name << "\""
      << " SET " << field_sql_name << " = \"" << value << "\""
      << " WHERE id=" << id;
  DD(sql.str());
  //  update "syd::Patient" set dicom_patientid="BIDON" where id=21;

  {
    auto odb_db = db->GetODB_DB();
    odb::transaction t (odb_db->begin ());
    odb_db->execute (sql.str());
    t.commit ();
    db->QueryOne(r, "Patient", id); // need to reload !
    DD(r);
  }

  /*
    - get the table from table name
    - object by id
    - perform sql "update" using sql table_name and sql field name
    - check return, get result
   */

  // ------------------------------------------------------------------
  if (0) {
    // get count
    DD(db->GetNumberOfElements("File"));
    DD(db->GetNumberOfElements<syd::File>());
    DD(db->GetNumberOfElements<syd::Image>());
    DD(db->GetNumberOfElements<syd::DicomFile>());
  }

  // ------------------------------------------------------------------
  if (0) {
    std::string filename="/Users/dsarrut/src/images/synfrizz3/db/synfrizz.db";
    auto db = new odb::sqlite::database(filename, SQLITE_OPEN_READWRITE, true);
    DD("done");

    odb::schema_version v (db->schema_version ());
    DD(v);
    // odb::schema_version cv (odb::schema_catalog::current_version (*db));
    // DD("ici");
    // odb::schema_version bv (odb::schema_catalog::base_version (*db));

    // DD(v);
    // DD(bv);
    // DD(cv);

    odb::transaction t (db->begin ());
    odb::schema_catalog::create_schema (*db);
    DD("here");
    // odb::transaction t (db->begin ());
    // odb::schema_catalog::migrate(*db);
    t.commit ();
  }

  // ------------------------------------------------------------------
  if (0) {

    // Load plugin
    syd::PluginManager::GetInstance()->Load();
    syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
    syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

    db->Dump("Image");

    syd::Image::vector images;
    db->Query(images);

    for(auto image:images) {
      DD(image);
      image->frame_of_reference_uid = image->dicoms[0]->dicom_frame_of_reference_uid;
      DD(image);
    }
    db->Update(images);
  }

  // ------------------------------------------------------------------
  DD("end");
  // This is the end, my friend.
}
// --------------------------------------------------------------------
