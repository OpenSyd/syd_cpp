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
#include "sydUpdateImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydScaleImageBuilder.h"
#include "sydImageHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydUpdateImage, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the ids
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=0; i<args_info.inputs_num; i++)
    ids.push_back(atoi(args_info.inputs[i]));

  // Get the images to udpate
  syd::Image::vector images;
  db->Query(images, ids);

  // Check
  if (args_info.file_given) {
    if (images.size() != args_info.file_given) {
      LOG(FATAL) << "You must provide as many --file as the number of image to update. Here there are "
                 << images.size() << " images and " << args_info.file_given << " --file option.";
    }
  }

  if (images.size() == 0) {
    LOG(1) << "No images, do nothing.";
  }

  // Check & updates the images
  int i=0;
  syd::ScaleImageBuilder builder(db);
  for(auto index=0; index<images.size(); index++) {
    auto image = images[index];
    // check file exist
    for(auto file:image->files) {
      std::string s = file->GetAbsolutePath();
      if (!fs::exists(s)) {
        LOG(WARNING) << "Image: " << image << std::endl
                     << "--> the file '" << s << "' does not exist." << std::endl;
        continue;
      }
    }

    // Need to import a new mhd ?
    if (args_info.file_given) {
      std::string mhd = args_info.file_arg[i];
      syd::ImageHelper::InsertMhdFiles(image, mhd);
    }

    // Need to scale ?
    double s = 1.0;
    if (args_info.scale_given) {
      s = args_info.scale_arg;
      if (args_info.squared_scale_flag) s = s*s;
      builder.Scale(image, s);
    }

    // update db
    syd::ImageHelper::UpdateImagePropertiesFromCommandLine(image, args_info);
    db->UpdateTagsFromCommandLine(image->tags, args_info);
    db->Update(image);
    if (s != 1) LOG(1) << "Image was scaled by " << s << ": " << image;
    else LOG(1) << "Image was updated: " << image;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
