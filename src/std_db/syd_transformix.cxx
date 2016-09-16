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
#include "syd_transformix_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"
#include "sydElastixHelper.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(syd_transformix, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Read ids from the command line and the pipe
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=0; i<args_info.inputs_num; i++)
    ids.push_back(atoi(args_info.inputs[i]));
  if (ids.size() < 1) {
    LOG(FATAL) << "Please provide at least one image id.";
  }
  syd::Image::vector initial_images;
  db->Query(initial_images, ids);

  // Read the Elastix
  std::vector<syd::IdType> tids;
  std::vector<std::string> stids;
  syd::GetWords(stids, args_info.elastix_arg);
  for(auto s:stids) tids.push_back(atoi(s.c_str()));
  syd::Elastix::vector initial_transforms;
  db->Query(initial_transforms, tids);

  // Match
  syd::Image::vector images;
  syd::Elastix::vector transforms;
  for(auto i=0; i<initial_images.size(); i++) {
    syd::Image::pointer image = initial_images[i];
    // Find the corresponding transforms
    int found = -1;
    for(auto j=0; j<initial_transforms.size(); j++) {
      if (initial_transforms[j]->moving_image->frame_of_reference_uid == image->frame_of_reference_uid) {
        if (found != -1) {
          LOG(FATAL) << "Error two transform could be used with the image: " << image << std::endl
                     << "First  is: " << initial_transforms[found] << std::endl
                     << "Second is: " << initial_transforms[j] << std::endl;
        }
        else found = j;
      }
    }
    if (found == -1) {
      LOG(WARNING) << "Could not find adequate Elastix for image (ignore):" << image;
    }
    else {
      images.push_back(image);
      transforms.push_back(initial_transforms[found]);
    }
  }

  // Display
  for(auto i=0; i<images.size(); i++) {
    syd::Image::pointer image = images[i];
    syd::Elastix::pointer elastix = transforms[i];
    LOG(1) << "Warp image " << image
           << " with " << elastix;
  }

  // Loop
  for(auto i=0; i<images.size(); i++) {
    syd::Image::pointer image = images[i];
    syd::Elastix::pointer elastix = transforms[i];

    // Check frame_of_reference_uid
    // if (!args_info.do_not_check_frame_of_reference_uid_flag) {
    if (elastix->moving_image->frame_of_reference_uid != image->frame_of_reference_uid) {
      LOG(WARNING) << "Frame_of_reference_uid does not correspond. Skip.";
      continue;
    }
    // }

    auto output = syd::InsertTransformixImage(elastix, image,
                                              args_info.default_pixel_arg,
                                              args_info.options_arg,
                                              args_info.verbose_arg);
    if (output) {
      // Allow the user to modify the information
      syd::SetImageInfoFromCommandLine(output, args_info);
      syd::SetTagsFromCommandLine(output->tags, db, args_info);
      db->Update(output);
      LOG(1) << "Image computed. Result: " << output;
    }
  }
  // This is the end, my friend.
}
// --------------------------------------------------------------------
