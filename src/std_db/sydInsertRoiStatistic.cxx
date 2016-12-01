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
#include "sydInsertRoiStatistic_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"
#include "sydRoiMaskImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydRoiStatisticHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertRoiStatistic, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of images
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=1; i<args_info.inputs_num; i++) { // start at 1 (because 0 is roi name)
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::Image::vector images;
  syd::FitImages::vector tias;

  std::string fit_output_name = args_info.fit_image_name_arg;

  if (!args_info.tia_flag) db->Query(images, ids);
  else {
    db->Query(tias, ids);
    for(auto tia:tias) images.push_back(tia->GetOutput(fit_output_name)); 
  }
  if (images.size() == 0) {
    LOG(FATAL) << "No image ids given. I do nothing.";
  }

  // Loop on images
  int i = 0;
  for(auto image:images) {
    LOG(3) << "Processing image " << image;
    // Consider the masks
    syd::RoiMaskImage::vector masks;
    if (args_info.inputs[0] == std::string("all")) {
      typedef odb::query<syd::RoiMaskImage> Q;
      Q q = Q::frame_of_reference_uid == image->frame_of_reference_uid;
      db->Query(masks, q);
    }
    else {
      if (args_info.inputs[0] == std::string("null")) {
        masks.push_back(NULL);
      }
      else {
        // find all masks with the same roi_name and frame_of_reference_uid
        masks = syd::FindRoiMaskImages(image, args_info.inputs[0]);
      }
    }

    if (masks.size() == 0) {
      LOG(WARNING) << "No masks found for this image, I do nothing: " << image;
    }

    // Loop over masks
    syd::RoiStatistic::pointer stat;
    for(auto mask:masks) {
      LOG(4) << "Processing mask " << mask;

      /*
      auto stat = syd::FindRoiStatistic(image, mask);
      if (!stat) stat = syd::NewRoiStatistic(image, mask);
      syd::ComputeRoiStatistic(stat, mask2, output);
      db->InsertOrUpdate(stat);
      */

      enum Mode {create, update, skip};
      Mode mode=create;

      if (!args_info.force_flag) {
        // Check if similar RoiStatistic already exist
        auto e = syd::FindRoiStatistic(image, mask);
        if ((e.size() == 1) and (args_info.update_flag)) {
          mode = update;
          stat = e[0];
        } else {
          if (e.size() != 0) mode = skip;
        }
        if (mode == skip) {
          std::stringstream ss;
          for(auto ee:e) ss << ee << " ";
          LOG(WARNING) << "Similar RoiStatistic already exists, I skip: " << ss.str();
          continue;
        }
      }

      // Get the mask filename (if needed)
      std::string mask_filename = "";
      if (args_info.resampled_mask_given)
        mask_filename = mask->roitype->name+"_"+std::to_string(image->id)
          +"_"+args_info.resampled_mask_arg;

      // Compute RoiStatistic
      syd::Image::pointer mask2 =nullptr;
      if (args_info.tia_flag) {
        mask2 = tias[i]->GetOutput("fit_1");
      }
      if (mode == update)
        syd::ComputeRoiStatistic(stat, mask2, mask_filename);
      else
        stat = syd::NewRoiStatistic(image, mask, mask2, mask_filename);

      // copy comments from the tia
      if (args_info.tia_flag and mode == create) {
        for(auto c:tias[i]->comments)
          stat->comments.push_back(c);
      }

      // Set the command lines tags
      syd::SetTagsFromCommandLine(stat->tags, db, args_info);
      syd::SetCommentsFromCommandLine(stat->comments, db, args_info);

      if (mode == update) {
        db->Update(stat);
        LOG(1) << "Update RoiStatistic: " << stat;
      }
      else  {
        db->Insert(stat);
        LOG(1) << "Insert RoiStatistic: " << stat;
      }
    }
    ++i;
  }
}
// --------------------------------------------------------------------
