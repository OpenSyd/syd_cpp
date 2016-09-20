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
#include "sydCalibrateImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydPixelUnitHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydCalibrateImage, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the ids
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=0; i<args_info.inputs_num; i++)
    ids.push_back(atoi(args_info.inputs[i]));

  // Get the images to udpate
  syd::Image::vector images;
  db->Query(images, ids);

  // Sort according to date
  db->Sort(images);
  auto ref = images[0];

  // Check same injection
  if (ref->injection == NULL) {
    LOG(FATAL) << "No injection for the first image";
  }
  auto injection = ref->injection;
  for(auto image:images) {
    if (image->injection != injection) {
      LOG(FATAL) << "Images have different injections "
                 << std::endl << ref
                 << std::endl << image;
    }
  }

  // Compute the calibration factor
  double s = syd::ComputeActivityInMBqByDetectedCounts(ref);
  s = s * args_info.scale_arg;
  // scale activity to be by injected activiy
  s = s / injection->activity_in_MBq;
  // scale from MBq to Bq
  s = s*1000000;

  for(auto image:images) {
    auto output = syd::CopyImage(image);
    syd::ScaleImage(output, s);
    output->pixel_unit = syd::FindPixelUnit(db, "Bq_by_IA[MBq]");
    syd::SetImageInfoFromCommandLine(output, args_info);
    syd::SetTagsFromCommandLine(output->tags, db, args_info);
    db->Update(output);
    LOG(1) << "Image was scaled by " << s << ": " << output;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
