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
  builder.SetImageActivityThreshold(args_info.min_activity_arg);
  builder.SetOptions(options);
  if (args_info.debug_images_in_db_flag) args_info.debug_images_flag = true;
  builder.SetDebugOutputFlag(args_info.debug_images_flag);

  // Go !
  builder.Run();

  // Results
  auto output = builder.InsertOutputImage();
  syd::SetImageInfoFromCommandLine(output, args_info);
  syd::SetTagsFromCommandLine(output->tags, db, args_info);
  db->Update(output);
  LOG(1) << "Time Integrated Image (tia) is: " << output;
  double n = builder.GetFilter().GetNumberOfPixels();
  double s = builder.GetFilter().GetNumberOfSuccessfullyFitPixels();
  LOG(1) << "Successfully fit pixels: "
         << s << "/" << n << " (" << (s/n*100.0) << "%)";

  // Debug output
  if (args_info.debug_images_flag) {
    if (!args_info.debug_images_in_db_flag)
      builder.WriteDebugOutput(); // write files
    else { // insert image in the db
      auto outputs = builder.InsertDebugOutputImages();
      // Output other images if needed
      for(auto o:outputs) {
        if (o->id != output->id) {
          LOG(2) << "\tDebug images: " << o;
        }
      }
    }
  }
  // This is the end, my friend.
}
// --------------------------------------------------------------------
