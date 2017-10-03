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
#include "sydInsertRoiTimepoints_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydTimepointsHelper.h"


// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertRoiTimepoints, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of RoiStatistic
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=0; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::RoiStatistic::vector stats;
  db->Query(stats, ids);
  if (stats.size() == 0) {
    LOG(1) << "No RoiStatistic.";
    return EXIT_SUCCESS;
  }

  // Check RoiStatistic
  auto injection = stats[0]->image->injection;
  auto frame_of_reference_uid = stats[0]->image->frame_of_reference_uid;
  auto pixel_unit = stats[0]->image->pixel_unit;
  for(auto s:stats) {
    if (s->image->frame_of_reference_uid != frame_of_reference_uid) {
      LOG(FATAL) << "Error different frame_of_reference_uid : "
                 << std::endl << stats[0]
                 << std::endl << s;
    }
    if (s->image->pixel_unit->id != pixel_unit->id) {
      LOG(FATAL) << "Error different pixel_unit : "
                 << std::endl << stats[0]
                 << std::endl << s;
    }
  }

  // Sort by time not needed -> done in NewRoiTimepoints

  // Check if already exist
  syd::RoiTimepoints::vector temp;
  syd::RoiTimepoints::pointer tp;
  odb::query<syd::RoiTimepoints> q =
    odb::query<syd::RoiTimepoints>::injection == injection->id;
  db->Query(temp, q);
  bool update = false;
  for(auto t:temp) {
    if (std::equal(t->roi_statistics.begin(), t->roi_statistics.end(), stats.begin())) {
      tp = t;
      update = true;
    }
  }

  // Create the RoiTimepoints
  if (!tp) tp = syd::NewRoiTimepoints(stats);
  else syd::UpdateRoiTimepoints(stats, tp);
  syd::SetTagsFromCommandLine(tp->tags, db, args_info);
  syd::SetCommentsFromCommandLine(tp->comments, db, args_info);
  if (!update) {
    db->Insert(tp);
    LOG(1) << "Insert new RoiTimepoints: " << tp;
  }
  else {
    db->Update(tp);
    LOG(1) << "Update RoiTimepoints: " << tp;
  }
  DD("done");

  // This is the end, my friend.
}
// --------------------------------------------------------------------
