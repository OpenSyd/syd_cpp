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
    int v;
    try {
      v = std::stoi(s);
      ids.push_back(v);
    } catch(std::exception & e) {
      roi_names.push_back(s);
    }
  }
  syd::FitImages::vector tias;
  db->Query(tias, ids);
  if (tias.size() == 0) {
    LOG(1) << "No FitImages.";
    return EXIT_SUCCESS;
  }

  // Loop on FitImages
  for(auto tia:tias) {
    // loop on roi_names
    for(auto & roi_name:roi_names) {

      // Consider the masks
      syd::RoiMaskImage::vector masks;
      if (roi_name == "all") {
        typedef odb::query<syd::RoiMaskImage> Q;
        Q q =
          Q::patient == tia->images[0]->patient->id and
          Q::frame_of_reference_uid == tia->images[0]->frame_of_reference_uid;
        db->Query(masks, q);
      }
      else {
        // find all masks with the same roi_name and frame_of_reference_uid
        masks = syd::FindRoiMaskImages(tia->images[0], roi_name);
      }

      if (masks.size() == 0) {
        LOG(WARNING) << "No masks found for this image, I do nothing: " << tia->images[0];
      }

      // Loop on mask
      for(auto mask:masks) {


        // Insert roi stats for every images
        syd::RoiStatistic::vector stats;
        for(auto image:tia->images) {

          // RoiStatistic
          auto e = syd::FindRoiStatistic(image, mask);
          syd::RoiStatistic::pointer stat;
          if (e.size() == 1) stat = e[0]; // if already exist, do not recompute
          if (e.size() == 0) { // if not compute it
            stat = syd::NewRoiStatistic(image, mask);
            LOG(1) << "Insert RoiStatistic: " << stat;
            db->Insert(stat);
          }
          if (e.size() >  1) {
            for(auto ee:e) std::cout << ee << std::endl;
            LOG(FATAL) << "Error several RoiStatistic exist for this image/mask pair.";
          }
          stats.push_back(stat);
        }

        // Timepoints
        syd::Timepoints::pointer rtp;
        auto ertp = syd::FindRoiTimepoints(stats);
        if (ertp.size() == 1) rtp = ertp[0];
        if (ertp.size() == 0) {
          rtp = syd::NewTimepoints(stats); // RoiTimepoints
          LOG(1) << "Insert Timepoints: " << rtp;
          db->Insert(rtp);
        }
        if (ertp.size() > 1) {
          for(auto e:ertp) std::cout << e << std::endl;
          LOG(FATAL) << "Error several RoiTimepoints exist for this list of stats.";
        }

        // Fit
        syd::FitTimepoints::pointer res;
        auto options = tia->GetOptions();
        auto eres = syd::FindFitTimepoints(rtp, options);
        if (eres.size() == 0) {
          res = syd::NewFitTimepoints(rtp, options);
          db->Insert(res);
          LOG(1) << "Insert FitTimepoints: " << options << " " << mask->roitype->name << " " << res;
        }
        if (eres.size() == 1) {
          res = eres[0];
          syd::ComputeFitTimepoints(res);
          db->Update(res);
          LOG(1) << "Update FitTimepoints: " << options << " " << mask->roitype->name << " " << res;
        }
        if (eres.size() > 1) {
          LOG(FATAL) << "Error several FitTimepoints exist.";
        }

      } // end loop mask
    }
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
