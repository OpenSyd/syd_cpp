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
#include "sydUpdateImageTag_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydUpdateImageTag, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the action (add or remove)
  std::string action = args_info.inputs[0];
  if (action != "add" and action != "rm" and action != "view") {
    LOG(FATAL) << "Please provide 'add' or 'rm' as second parameter.";
  }

  // Get the tags
  std::string tagname = args_info.inputs[1];
  syd::Tag::vector tags;
  int start = 2;
  if (action != "view") {
    db->FindTags(tags, tagname);
    if (tags.size() == 0) {
      LOG(1) << "No found tag from '" << tagname << "'";
      return EXIT_SUCCESS;
    }
  }
  else start = 1;

  // Read the standard input if pipe
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);

  // Get the list of image ids
  syd::Image::vector images;
  for(auto i=start; i<args_info.inputs_num; ++i) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  db->Query(images, ids);

  // Change the tag
  for(auto & i:images) {
    if (action == "add") for(auto t:tags) i->AddTag(t);
    else {
      if (action == "rm") for(auto t:tags) i->RemoveTag(t);
      else {
        if (action == "view") {
          std::cout << i->id << " ";
          for(auto t:i->tags) std::cout << t->label << " ";
        }
      }
    }
    if (action != "view") LOG(1) << "Change tag for image " << i;
    else std::cout << std::endl;
  }
  // Update the db
  if (images.size() > 0) db->Update(images);
  else {
    LOG(1) << "No tag changed.";
  }

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
