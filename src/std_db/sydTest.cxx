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
#include "sydTableOfRecords.h"

#include "sydTestTemp3.h"
#include "sydTestTemp4.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydTest, 1);

  // Load plugin and db
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // -----------------------------------------------------------------
  {
    std::shared_ptr<temp::Patient> p(new temp::Patient);
    p->name = "toto";
    p->id = 1;
    std::shared_ptr<temp::Record> r = p;
    DD(p->GetTraits()->GetTableName());
    DD(r->GetTraits()->GetTableName());
    temp::RecordTraits<temp::Patient>::GetTraits()->GetTableName();
    std::shared_ptr<temp::Record> b(new temp::Record);
    b->id = 2;
    DD(b->GetTraits()->GetTableName());

    DD("-----------------");
    DD(p->GetTableName());
    DD(r->GetTableName());
    DD(b->GetTableName());

    DD("-----------------");
    auto f = p->GetFieldFunction("name");
    auto g = r->GetFieldFunction("id");
    auto h = b->GetFieldFunction("id");
    try {
      auto i = b->GetFieldFunction("name");
    } catch(std::exception & e) {
      std::cout << "OK name does not exists" << std::endl;
    }

    DD("-----------------");
    DD(f(p));
    DD(f(r));
    DD(f(b)); // !!  BUG --> dyn cast required 

    DD("-----------------");
    DD(g(p));
    DD(g(r));
    DD(g(b));

    DD("-----------------");
    DD(h(p));
    DD(h(r));
    DD(h(b));

    DD("-----------------");

  }


  {
    DD("=====================");
    syd::Record::vector records;
    db->Query(records, "Injection"); // query all

    syd::FieldBase::vector fields;
    auto traits = records[0]->GetTraits();  // consider only the first one
    //auto traits = GetTraits("Injection"); // alternative
    fields.push_back(traits->GetField("id"));
    fields.push_back(traits->GetField("date"));
    fields.push_back(traits->GetField("activity_in_MBq", 2));
    fields.push_back(traits->GetField("patient->name"));

    for(auto f:fields) {
      DD(f->GetValue(records[0]));
    }

  }

  exit(0);


  // ------------------------------------------------------------------
  {
    /*
      syd::Patient::vector patients;
      db->Query(patients);
      syd::Record::vector records;
      for(auto p:patients) records.push_back(p); // maybe at templated function in TableOfRecords

      auto g = patients[0]->GetFieldFunction2("name");
      DD(g(patients[0]));
      DD(g(records[0]));

      auto f = records[0]->GetFieldFunction2("name");
      DD(f(patients[0]));
      DD(f(records[0]));

      auto h = syd::Patient::GetFieldFunction2("name");
      DD(h(patients[0]));
      DD(h(records[0]));
    */

    /*    auto g = db->GetFieldFunction("Patient", "name");
          DD(g(patients[0]));
          DD(g(records[0]));
    */

  }



  // ------------------------------------------------------------------
  syd::Patient::vector patients;
  db->Query(patients);
  // DDS(patients);

  syd::Record::vector records;
  for(auto & p:patients) records.push_back(p); // maybe at templated function in TableOfRecords

  syd::TableOfRecords table;
  table.Set(records);
  table.AddField("name");
  table.AddField("id");
  table.AddField("dicom");
  table.AddField("study_id");
  table.Print(std::cout);

  syd::Image::vector images;
  db->Query(images);
  records.clear();
  for(auto & p:images) records.push_back(p);

  table.Set(records);
  table.ClearFieldList();
  table.AddField("id");
  table.AddField("pname");
  table.AddField("date");
  table.AddField("inj");
  table.AddField("injq");
  table.Print(std::cout);

  // ------------------------------------------------------------------
  DD("end");
  // This is the end, my friend.
}
// --------------------------------------------------------------------
