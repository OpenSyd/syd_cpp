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
  SYD_INIT_GGO(sydInsertRoiStatistic, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of images
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=1; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::Image::vector images;
  db->Query(images, ids);
  if (images.size() == 0) {
    LOG(FATAL) << "No image ids given. I do nothing.";
  }

  // Loop on images
  for(auto image:images) {

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
        masks = syd::FindRoiMaskImage(image, args_info.inputs[0]);
      }
    }

    // Loop over masks
    syd::RoiStatistic::pointer stat;
    for(auto mask:masks) {
      std::string mask_filename = "";
      if (args_info.resampled_mask_given)
        mask_filename = mask->roitype->name+"_"+std::to_string(image->id)
          +"_"+args_info.resampled_mask_arg;
      DD(mask_filename);
      stat = syd::NewRoiStatistic(image, mask, mask_filename);
      DD(stat);
      // Tags
      syd::SetTagsFromCommandLine(stat->tags, db, args_info);
      syd::SetCommentsFromCommandLine(stat->comments, db, args_info);

      // Check already exist something similar ?? FIXME 

      db->Insert(stat);
      LOG(1) << "Insert RoiStatistic: " << stat;
    }
  }
}
// --------------------------------------------------------------------
