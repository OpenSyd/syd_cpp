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
#include "sydCropImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCropImageBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT(sydCropImage, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the image
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=1; i<args_info.inputs_num; i++) ids.push_back(atoi(args_info.inputs[i]));
  if (ids.size() == 0) return EXIT_SUCCESS;

  // Option like or threshold
  double t = args_info.threshold_arg;
  syd::Image::pointer like;
  if (args_info.like_given) {
    db->QueryOne(like, args_info.like_arg);
  }

  // Crop
  syd::Image::vector images;
  db->Query(images, ids);
  if (images.size() == 0) {
    LOG(1) << "No image to crop";
  }

  syd::CropImageBuilder b(db);
  for(auto image:images) {
    auto size = image->size;
    //b.CropImageLike(image, like, false);
    if (args_info.like_given) b.CropImageLike(image, like, args_info.force_flag);
    else {
      if (args_info.threshold_given) b.CropImageWithThreshold(image, t);
      else {
        if (image->dicoms.size() == 0) {
          LOG(WARNING) << "No associated dicom for this image, no --like nor -t given, I do nothing";
        }
        // Try to find the body of this image
        syd::RoiType::pointer body = db->FindRoiType("body");
        try {
          auto mask = db->FindRoiMaskImage(body, image->dicoms[0]);
          LOG(1) << "Find 'body' mask for the image: " << mask;
          b.CropImageLike(image, mask, args_info.force_flag);
        } catch (std::exception & e) {
          LOG(WARNING) << "Could not crop the image: " << image
                       << std::endl << "Error is: " << e.what();
          continue; // (skip log)
        }
      }
    }
    LOG(1) << "Image cropped: " << image << " (initial size was " << syd::ArrayToString<int,3>(size) << ")";
  }


  // This is the end, my friend.
}
// --------------------------------------------------------------------
