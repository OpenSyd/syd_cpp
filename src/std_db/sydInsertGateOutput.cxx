/*=========================================================================
  Program:   sfz

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
#include "sydInsertGateOutput_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"
#include "sydGateHelper.h"
#include "sydPixelUnitHelper.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydInsertGateOutput, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the folder
  std::string folder_name = args_info.inputs[0];

  // Get the injection
  syd::IdType id = atoi(args_info.inputs[1]);
  auto source = db->QueryOne<syd::Image>(id);

  // Get the images (dose+edep+uncert)
  auto images = syd::GateInsertOutputImages(folder_name, source);
  DDS(images);

  // Get the stat file
  auto stat_file = syd::GateInsertStatFile(folder_name, source->patient);
  if (stat_file != nullptr) {
    DD(stat_file);
    double nb_events = syd::GateGetNumberOfEvents(stat_file);

    // Scale the image to get it in cGy by injection MBq
    double s = syd::GateComputeDoseScalingFactor(source, nb_events);
    double cGy = 100.0;
    s = s * cGy;
    auto unit = syd::FindOrCreatePixelUnit(db, "cGy/IA[MBq]");
    for(auto & image:images) {
      syd::ScaleImage(image, s);
      image->pixel_unit = unit;
    }
    DDS(images);
  }
  else {
    LOG(WARNING) << "Cannot find stat file, no scaling.";
  }

  // Tags and info from cmd line
  for(auto & image:images) {
    syd::SetTagsFromCommandLine(image->tags, db, args_info);
    syd::SetImageInfoFromCommandLine(image, args_info);
    syd::SetCommentsFromCommandLine(image->comments, db, args_info);
  }
  db->Update(images);

  LOG(1) << "Inserted: " << images.size() << " images.";
  for(auto & image:images) {
    LOG(2) << image;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
