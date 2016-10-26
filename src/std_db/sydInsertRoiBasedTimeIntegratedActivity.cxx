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
#include "sydInsertRoiBasedTimeIntegratedActivity_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"
#include "sydTimeIntegratedActivityImageBuilder.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydRoiMaskImageHelper.h"
#include "sydRoiStatisticHelper.h"
#include "sydTimepointsHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertRoiBasedTimeIntegratedActivity, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of FitImages to get the fit options + the images
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  std::vector<std::string> roi_names;
  for(auto i=0; i<args_info.inputs_num; i++) {
    std::string s = args_info.inputs[i];
    DD(s);
    int v;
    try {
      v = std::stoi(s);
      DD(v);
      ids.push_back(v);
    } catch(std::exception & e) {
      roi_names.push_back(s);
    }
  }
  DDS(ids);
  DDS(roi_names);
  syd::FitImages::vector tias;
  db->Query(tias, ids);
  if (tias.size() == 0) {
    LOG(1) << "No FitImages.";
    return EXIT_SUCCESS;
  }
  DDS(tias);


  // Loop on FitImages
  for(auto tia:tias) {
    DD(tia);

    // loop on roi_names
    for(auto & roi_name:roi_names) {
      // Consider the masks
      syd::RoiMaskImage::vector masks;
      if (roi_name == "all") {
        typedef odb::query<syd::RoiMaskImage> Q;
        Q q = Q::frame_of_reference_uid == tia->images[0]->frame_of_reference_uid;
        db->Query(masks, q);
      }
      else {
        // find all masks with the same roi_name and frame_of_reference_uid
        masks = syd::FindRoiMaskImage(tia->images[0], roi_name);
      }
      DDS(masks);

      if (masks.size() == 0) {
        LOG(WARNING) << "No masks found for this image, I do nothing: " << tia->images[0];
      }

      // Loop on mask
      for(auto mask:masks) {
        DD(mask);

        // Insert roi stats for every images
        syd::RoiStatistic::vector stats;
        for(auto image:tia->images) {
          auto stat = syd::NewRoiStatistic(image, mask);
          stats.push_back(stat);
          DD(stat);
        }
        for(auto stat:stats) {
          auto s = syd::FindSameRoiStatistic(stat);
          if (s != nullptr) {
            LOG(WARNING) << "Same RoiStatistic already exists, I skip it: " << s;
          }
          else {
            LOG(1) << "Insert RoiStatistic: " << stat;
            db->Insert(stat);
          }
        }

        // Insert a new timepoints
        auto rtp = syd::NewTimepoints(stats); // RoiTimepoints
        DD(rtp);
        DD("check already exist ?");

        // Fit 
        auto options = tia->GetOptions();
        auto res = syd::NewFitTimepoints(rtp, options);
        DD(res);
        DD("check already exist ?");

      } // end loop mask
    }
  }
  DD("end");


  // // get models names
  // std::vector<std::string> model_names;
  // for(auto i=0; i<args_info.model_given; i++)
  //   model_names.push_back(args_info.model_arg[i]);
  // if (model_names.size() == 0)
  //   model_names.push_back("f4"); // default model
  // DD(model_names);

  // // Fit options
  // syd::TimeIntegratedActivityFitOptions options;
  // options.SetRestrictedFlag(args_info.restricted_tac_flag);
  // options.SetR2MinThreshold(args_info.r2_min_arg);
  // options.SetMaxNumIterations(args_info.iterations_arg);
  // options.SetAkaikeCriterion(args_info.akaike_arg);
  // for(auto m:model_names) options.AddModel(m);
  // //  options.AddTimeValue(0,0);
  // // options.AddTimeValue(0,0);

  // // Main builder
  // syd::TimeIntegratedActivityImageBuilder builder;
  // builder.SetInput(images);
  // builder.SetImageActivityThreshold(args_info.min_activity_arg);
  // builder.SetOptions(options);
  // builder.SetDebugOutputFlag(args_info.debug_images_flag);

  // // Go !
  // auto tia = builder.Run();
  // syd::SetCommentsFromCommandLine(tia->comments, db, args_info);
  // db->Insert(tia);

  // // Results
  // for(auto output:tia->outputs) {
  //   syd::SetImageInfoFromCommandLine(output, args_info);
  //   syd::SetTagsFromCommandLine(output->tags, db, args_info);
  //   syd::SetCommentsFromCommandLine(output->comments, db, args_info);
  //   db->Update(output);
  // }
  // auto s = tia->nb_pixels;
  // auto n = tia->nb_success_pixels;
  // LOG(1) << "Time Integrated Activity: " << tia;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
