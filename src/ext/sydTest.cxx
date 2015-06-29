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
#include "extExtendedDatabase.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(sydTest, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  DD(dbname);

  {
    std::string b = dbname+".backup";
    std::rename(dbname.c_str(), b.c_str());
    std::string folder = "test";
    //m->Create("StandardDatabase", dbname, folder);
    m->Create("ExtendedDatabase", dbname, folder);
  }


  std::vector<std::string> args;
  args.push_back("toto");
  args.push_back("1");
  args.push_back("77");
  args.push_back("XYXYXYX");
  args.push_back("2015-654-354");


  // With generic db
  {

    DD("------------------");
    syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);
    db->Dump(std::cout); // list of tables etc

    // Create a new record

    auto patient = db->NewPatient();
    DD(patient);
    db->Set(patient, args);
    DD(patient);
    db->Insert(patient);
    DD(patient);

    auto p = db->FindPatient("toto");
    DD(p);

    auto r = db->Find("Patient", 1);
    DD(r);

    try {
      db->FindPatient("titi");
    }
    catch(std::exception & e) {
      std::cout << "titi not found. " << e.what();
    }

    // DD("----------------");
    // auto inj = db->NewRecord("Injection");
    // DD(inj);
    // //    inj->patient = record;
    // args.clear();
    // args.push_back("toto");
    // args.push_back("1");
    // args.push_back("bidon");
    // DDS(args);
    // db->Set(inj, args);
    // //inj->Set(db, args); // same previous, but 'less' consistant
    // DD(inj);
    // db->InsertRecord(inj);
    // DD(inj);


    //    get sqldb try persist here
    /*
    auto s = db->GetSQLDatabase();
    odb::transaction t (s->begin());
    //  std::shared_ptr<syd::Record> a(new syd::Patient);
    std::shared_ptr<syd::Record> a(syd::Patient::New());
    //    syd::Record * a(syd::Patient::New());
    DD(a);
    s->persist(a);
    //    db_->update(r);
    t.commit();
    DD(a);
    */
  }


  // This is the end, my friend.
}
// --------------------------------------------------------------------
