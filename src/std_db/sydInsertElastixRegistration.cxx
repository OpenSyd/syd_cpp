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
#include "sydInsertElastixRegistration_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"
#include "sydRoiMaskImageHelper.h"
#include "sydFileHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertElastixRegistration, 1);

  // Load plugin and get db
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the elastix config file
  std::string config_filename = args_info.inputs[0];

  // Read ids from the command line and the pipe
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=1; i<args_info.inputs_num; i++)
    ids.push_back(atoi(args_info.inputs[i]));
  if (ids.size() < 2) {
    LOG(FATAL) << "Please provide at least two image ids (fixed and moving  images).";
  }

  // Get the reference image
  syd::Image::pointer fixed_image;
  db->QueryOne(fixed_image, ids[0]);
  auto patient = fixed_image->patient;

  // Get the masks
  syd::RoiMaskImage::pointer fixed_mask;
  if (args_info.fMask_given) {
    fixed_mask = syd::FindRoiMaskImage(fixed_image, args_info.fMask_arg);
  }

  // Loop over all images
  for(auto i=1; i<ids.size(); i++) {

    syd::Image::pointer moving_image;
    db->QueryOne(moving_image, ids[i]);

    syd::RoiMaskImage::pointer moving_mask;
    std::string moving_mask_path;
    if (args_info.mMask_given) {
      moving_mask = syd::FindRoiMaskImage(moving_image, args_info.mMask_arg);
    }

    // Get elastix param
    std::string options = args_info.options_arg;

    // Create the object
    syd::Elastix::pointer elastix;
    db->New(elastix);
    elastix->fixed_image = fixed_image;
    elastix->fixed_mask = fixed_mask;
    elastix->moving_image = moving_image;
    elastix->moving_mask = moving_mask;
    elastix->options = options;
    db->Insert(elastix);

    auto temp = db->GetUniqueTempFilename();
    std::string folder = patient->name+PATH_SEPARATOR+"elastix";
    syd::File::pointer config = syd::NewFile(db, folder, temp);
    db->Insert(config);
    config->filename = "config_"+std::to_string(elastix->id)+"_"
      +fs::path(config_filename).filename().string();
    fs::copy(config_filename, config->GetAbsolutePath());
    elastix->config_file = config;
    db->Update(elastix);
    LOG(1) << "Insert elastix: " << elastix;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
