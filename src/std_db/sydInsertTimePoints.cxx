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
#include "sydInsertTimePoints_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydTimePointsBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertTimePoints, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Check pixelvalueunit
  //auto punit = db->FindPixelValueUnit(args_info.pixelunit_arg);

  // Get the list of images to integrate
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=0; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::Image::vector images;
  db->Query(images, ids);
  if (images.size() ==0) {
    LOG(1) << "No images.";
    return EXIT_SUCCESS;
  }

  // Get the mask
  std::string roi_name = args_info.inputs[0];
  syd::RoiMaskImage::pointer mask = db->FindRoiMaskImage(images[0], roi_name);
  DD(mask);

  // Create (or update) the tac
  syd::TimePoints::pointer tac;
  syd::TimePointsBuilder builder(db);
  builder.SetImages(images);
  builder.SetRoiMaskImage(mask);
  builder.ComputeTimePoints(tac);

  // Set tags
  DD(tac);

  // Insert or update
  //  if (tac->IsPersistent()) db->Update(tac);
  //else db->Insert(tac);


  // This is the end, my friend.
}
// --------------------------------------------------------------------
