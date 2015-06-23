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
#include "sydImageTag_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT(sydImageTag, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the action (add or remove)
  std::string action = args_info.inputs[1];
  if (action != "add" and action != "rm") {
    LOG(FATAL) << "Please provide 'add' or 'rm' as second parameter.";
  }

  // Get the tags
  std::string tagname = args_info.inputs[2];
  std::vector<syd::Tag> tags;
  syd::FindTags(tags, db, tagname);

  // Read the standard input if pipe
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);

  // Get the list of image ids
  std::vector<syd::Image> images;
  for(auto i=3; i<args_info.inputs_num; ++i) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  db->Query(ids, images);

  // Change the tag
  for(auto & i:images) {
    if (action == "add") for(auto t:tags) i.AddTag(t);
    else for(auto t:tags) i.RemoveTag(t);
    LOG(1) << "Change tag for image " << i;
  }
  // Update the db
  if (images.size() > 0) db->Update(images);
  else {
    LOG(1) << "No tag changed.";
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
