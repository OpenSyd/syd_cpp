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

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydTest, 1);

  // Load plugin and db
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

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
  // DDS(images);
  records.clear();
  for(auto & p:images) records.push_back(p);

  table.Set(records);
  table.AddField("name");
  table.AddField("dicom");
  table.AddField("study_id");
  table.Print(std::cout);


  // ------------------------------------------------------------------
  DD("end");
  // This is the end, my friend.
}
// --------------------------------------------------------------------
