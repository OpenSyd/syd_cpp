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
#include "sydUpdateTags_ggo.h"
#include "sydDatabaseManager.h"
#include "sydStandardDatabase.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydUpdateTags, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the ids
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=1; i<args_info.inputs_num; i++)
    ids.push_back(atoi(args_info.inputs[i]));

  // Get the table name
  std::string table_name = args_info.inputs[0];

  // Get the images to udpate
  syd::Record::vector records;
  db->Query(records, table_name, ids);

  for(auto & record:records) {
    auto r = std::dynamic_pointer_cast<syd::RecordWithTags>(record);
    if (r == nullptr) {
      LOG(FATAL) << "Error, the records of table " << record->GetTableName()
                 << " do not have tags";
    }
    syd::SetTagsFromCommandLine(r->tags, db, args_info);
  }

  db->Update(records, table_name);
  LOG(1) << records.size() << " records were updated.";

  // This is the end, my friend.
}
// --------------------------------------------------------------------
