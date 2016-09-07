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

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertTimeIntegratedActivityImage, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Check pixelvalueunit
  syd::PixelValueUnit::pointer punit;
  if (args_info.pixelunit_given)
    punit = db->FindPixelValueUnit(args_info.pixelunit_arg);
  else
    punit = db->FindPixelValueUnit("no_unit");

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

  // Get times
  db->Sort<syd::Image>(images);
  std::vector<double> times;
  for(auto im:images) times.push_back(im->GetHoursFromInjection());

  // debug fla
  bool debug=false;
  if (args_info.debug_images_in_db_flag) debug = true;
  if (args_info.debug_images_flag) debug = true;

  // main builder
  syd::TimeIntegratedActivityImageBuilder builder(db);
  builder.SetTimes(times);
  builder.SetInput(images);
  builder.SetPreProcessingGaussianFilter(args_info.gauss_arg);
  builder.SetMinimumValueMask(args_info.min_activity_arg);
  builder.SetR2MinThreshold(args_info.r2_min_arg);
  builder.SetRestrictedTACFlag(args_info.restricted_tac_flag);
  builder.SetModels(model_names);
  builder.SetDebugImagesFlag(debug);
  builder.SetPostProcessingMedianFilter(args_info.median_filter_flag);
  builder.SetPostProcessingFillHoles(args_info.fill_holes_arg);
  builder.SetInitialZeroPoint(args_info.add_initial_zero_flag);
  if (args_info.add_time_given and args_info.add_value_given)
    builder.SetAdditionalPoint(true, args_info.add_time_arg, args_info.add_value_arg);
  LOG(1) << builder.PrintOptions();
  // Go
  builder.CreateTimeIntegratedActivityImage();
  builder.RunPostProcessing();

  // Debug images
  if (debug) {
    typedef syd::TimeIntegratedActivityImageBuilder::ImageType ImageType;
    for(auto o:builder.GetOutputs()) o->WriteImage();
  }
  if (args_info.debug_images_in_db_flag) {
    std::vector<std::string> tag_names;
    for(auto i=0; i<args_info.tag_given; i++)
      tag_names.push_back(args_info.tag_arg[i]);
    builder.InsertOutputImagesInDB(tag_names);
  }

  // Insert in db
  syd::Image::pointer tia = builder.GetTimeIntegratedActivityImage();
  db->UpdateTagsFromCommandLine(tia->tags, args_info);
  tia->pixel_value_unit = punit;
  builder.InsertAndRename(tia);
  LOG(1) << "Inserting Image " << tia;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
