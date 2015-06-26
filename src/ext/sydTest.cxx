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
#include "sydStandardDatabase.h"
#include "extExtendedDatabase.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(sydTest, 1);

  Log::SQLFlag() = false;
  Log::LogLevel() = 10;

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
    //syd::Database * db = m->Create("StandardDatabase", dbname, folder);
    syd::Database * db = m->Create("ExtendedDatabase", dbname, folder);
    db->Dump();
  }


  // With generic db
  {
    /*
    DD("------------------");
    syd::Database * db = m->Read(dbname); // FIXME set it shared
    db->Dump(std::cout); // list of tables etc

    // Create a new record
    auto record = db->NewRecord("Patient");
    DD(record);
    std::vector<std::string> args;
    args.push_back("toto");
    args.push_back("1");
    db->Set(record, args);
    DD(record);
    db->Insert(record);
    */
  }

  // With specific db
  {

    DD("------------------");
    syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);
    db->Dump(std::cout);

    // Create a new patient
    //    syd::Patient * a = new syd::Patient; // MUST NOT BE POSSIBLE
    // ext::Patient * b = new ext::Patient;    // MUST NOT BE POSSIBLE

    auto patient = db->NewPatient();
    //    db->New(patient);
    std::cout << patient << std::endl;
    DD(patient);
    db->Insert(patient);
    DD(patient);

    // Read a patient
    /*
    auto patient = db->FindPatient("toto");
    DD(patient);
    db->Find(patient, id);
    DD(patient);
    */
  }


  // This is the end, my friend.
}
// --------------------------------------------------------------------
