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
#include "sydInsertTimeIntegratedActivityImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"
#include "sydTimeIntegratedActivityImageBuilder.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertTimeIntegratedActivityImage, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of images to integrate
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=0; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::Image::vector images;
  db->Query(images, ids);
  if (images.size() == 0) {
    LOG(1) << "No images.";
    return EXIT_SUCCESS;
  }
  if (images.size() == 1) {
    LOG(1) << "Cannot integrate a single image.";
    return EXIT_SUCCESS;
  }

  // get models names
  std::vector<std::string> model_names;
  for(auto i=0; i<args_info.model_given; i++)
    model_names.push_back(args_info.model_arg[i]);
  if (model_names.size() == 0)
    model_names.push_back("f4"); // default model

  // Fit options
  syd::TimeIntegratedActivityFitOptions options;
  options.SetRestrictedFlag(args_info.restricted_tac_flag);
  options.SetR2MinThreshold(args_info.r2_min_arg);
  options.SetMaxNumIterations(args_info.iterations_arg);
  options.SetAkaikeCriterion(args_info.akaike_arg);
  for(auto m:model_names) options.AddModel(m);
  //  options.AddTimeValue(0,0);
  // options.AddTimeValue(0,0);

  // Main builder
  syd::TimeIntegratedActivityImageBuilder builder;
  builder.SetInput(images);
  builder.SetMaskName(args_info.mask_arg);
  builder.SetImageActivityThreshold(args_info.min_activity_arg);
  builder.SetOptions(options);
  builder.SetDebugOutputFlag(args_info.debug_images_flag);

  // Go !
  auto tia = builder.Run();

  // Results
  syd::SetCommentsFromCommandLine(tia->comments, db, args_info);
  db->Insert(tia);
  for(auto output:tia->outputs) {
    syd::SetImageInfoFromCommandLine(output, args_info);
    syd::SetTagsFromCommandLine(output->tags, db, args_info);
    syd::SetCommentsFromCommandLine(output->comments, db, args_info);
    db->Update(output);
  }
  auto s = tia->nb_pixels;
  auto n = tia->nb_success_pixels;
  LOG(1) << "Time Integrated Activity: " << tia;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
