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


#define ASYD_VERSION      0x0304 // 01.02
#define ASYD_BASE_VERSION 0x0101 // 01.01


#define SFZ_VERSION      SYD_VERSION + 0x010000 // 01.xx.yy
#define SFZ_BASE_VERSION SYD_BASE_VERSION + 0x010000 // 01.xx.yy

#pragma db model version(SFZ_BASE_VERSION, SFZ_VERSION)

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

  // ------------------------------------------------------------------
  if (0) {
    db->CheckDatabaseSchema();
  }

  // ------------------------------------------------------------------
  if (0) {
    db->InitDatabaseDescription();
    auto desc = db->GetDatabaseDescription();
    DD(*desc);
  }

  // ------------------------------------------------------------------
  if (0) {
    // get count
    DD(db->GetNumberOfElements("File"));
    DD(db->GetNumberOfElements<syd::File>());
    DD(db->GetNumberOfElements<syd::Image>());
    DD(db->GetNumberOfElements<syd::DicomFile>());
  }

  // ------------------------------------------------------------------
  // Trial migration ?
  if (1) {
    // std::string filename="bidon.db";
    // std::string schema_name = "SynfrizzDatabase";
    // DD(filename);
    // DD(schema_name);
    // auto db = new odb::sqlite::database(filename, SQLITE_OPEN_READWRITE| SQLITE_OPEN_CREATE, true);
    DD(ASYD_VERSION);
    std::cout << std::hex << SYD_VERSION << std::endl;
    std::cout << std::hex
              << (ASYD_VERSION & 0xFF0000)/0xFFFF << "."
              << (ASYD_VERSION & 0x00FF00)/0xFF << "."
              << (ASYD_VERSION & 0x0000FF)
              << std::endl;

    DD(SFZ_VERSION);
    std::cout << std::hex << SFZ_VERSION << std::endl;

    std::cout << std::hex
              << (SFZ_VERSION & 0xFF0000)/0xFFFF << "."
              << (SFZ_VERSION & 0x00FF00)/0xFF << "."
              << (SFZ_VERSION & 0x0000FF)
              << std::endl;
    DD("open done");
        exit(0);

    DD(args_info.inputs[0]);
    // if (std::string(args_info.inputs[0]) =="1") {
    //   DD("CREATION");
    //   odb::connection_ptr c (db->connection ());
    //   //    c->execute ("PRAGMA foreign_keys=ON");
    //   odb::transaction t (db->begin ());
    //   odb::schema_catalog::create_schema(*db, schema_name, true);
    //   t.commit ();
    //   DD("create done");
    // }

    // odb::schema_version v (db->schema_version(schema_name));
    // DD(v);
    // odb::schema_version cv (odb::schema_catalog::current_version (*db, schema_name));
    // DD(cv);
    // odb::schema_version bv (odb::schema_catalog::base_version (*db, schema_name));
    // DD(bv);

    // odb::transaction t (db->begin ());
    // odb::schema_catalog::migrate(*db, 3, schema_name);
    // DD("here");
    // t.commit ();
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
