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
#include "sydCommonGengetopt.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydCropImage, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the image
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=0; i<args_info.inputs_num; i++) ids.push_back(atoi(args_info.inputs[i]));
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
    if (args_info.like_given) b.CropImageLike(image, like, args_info.resample_flag, args_info.force_flag);
    else {
      if (args_info.threshold_given) b.CropImageWithThreshold(image, t);
      else {
        if (image->dicoms.size() == 0) {
          LOG(WARNING) << "No associated dicom for this image, no --like nor -t given, I do nothing";
        }
        // Try to find the body of this image
        syd::RoiType::pointer body = db->FindRoiType("body");
        try {
          odb::query<syd::RoiMaskImage> q =
            odb::query<syd::RoiMaskImage>::patient == image->patient->id and
            odb::query<syd::RoiMaskImage>::roitype == body->id and
            odb::query<syd::RoiMaskImage>::frame_of_reference_uid == image->frame_of_reference_uid;
          syd::RoiMaskImage::pointer mask;
          db->QueryOne(mask, q);
          LOG(1) << "Find 'body' mask for the image: " << mask;
          b.CropImageLike(image, mask, args_info.resample_flag , args_info.force_flag); // false = dont resample
          db->UpdateTagsFromCommandLine(image->tags, args_info);
        } catch (std::exception & e) {
          LOG(WARNING) << "Could not crop the image: " << image
                       << std::endl << "Error is: " << e.what();
          continue; // (skip log)
        }
      }
    }

    // Final update and log
    db->Update(image);
    LOG(1) << "Image cropped: " << image
           << " (initial size was " << syd::ArrayToString<int,3>(size) << ")";
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
