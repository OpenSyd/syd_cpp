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
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"

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

    if (args_info.copy_flag) {
      auto copy = syd::CopyImage(image);
      image = copy;
    }

    if (args_info.gauss_arg != 0) {
      double sigma_in_mm = args_info.gauss_arg;
      syd::ApplyGaussianFilter(image, sigma_in_mm);
    }

    // Need to import a new mhd ?
    if (args_info.file_given) {
      if (image->type != "mhd") {
        LOG(WARNING) << "Image: " << image
                     << " not of type mhd. Skip." << std::endl;
        continue;
      }
      if (image->files.size() != 2) {
        LOG(WARNING) << "Image: " << image
                     << " does not have 2 associated files. Skip."
                     << std::endl;
        continue;
      }
      std::string mhd = args_info.file_arg[i];
      // delete current files
      std::string to_relative_path = image->files[0]->path;
      std::string to_filename = image->files[0]->filename;
      db->Delete(image->files);
      // Insert new files
      image->files = syd::InsertFilesFromMhd(db, mhd, to_relative_path, to_filename);
      // update image info
      syd::SetImageInfoFromFile(image);
    }

    // Need to scale ?
    double s = 1.0;
    if (args_info.scale_given) {
      s = args_info.scale_arg;
      if (s != 1.0) {
        syd::ScaleImage(image, s);
        LOG(1) << "Image was scaled by " << s << ": " << image;
      }
    }

    // Need to convert to another pixel_type ?
    if (args_info.pixel_type_given) {
      LOG(FATAL) << "Sorry, not yet implemented.";
      //syd::CastImage(image, args_info.pixel_type_arg);
    }

    // Need to fill empty pixels
    if (args_info.fill_holes_arg > 0) {
      syd::Image::pointer mask;
      syd::IdType id = args_info.fill_mask_image_arg;
      db->QueryOne(mask, id);
      double value = args_info.fill_mask_value_arg;
      int nb_fail;
      int nb_changed;
      syd::FillHoles(image, mask, args_info.fill_holes_arg, value, nb_fail, nb_changed);
      LOG(1) << "Fill holes terminated with " << nb_fail << " fails and " << nb_changed << " voxels changed.";
    }

    // update db
    syd::SetImageInfoFromCommandLine(image, args_info);
    syd::SetTagsFromCommandLine(image->tags, db, args_info);
    syd::SetCommentsFromCommandLine(image->comments, db, args_info);
    db->Update(image);
    LOG(1) << "Image was updated: " << image;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
