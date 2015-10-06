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
#include "sydInsertDecayCorrectedImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydDecayCorrectedImageBuilder.h"
#include "sydCommonGengetopt.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertDecayCorrectedImage, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the tag
  std::string tagname = args_info.inputs[0];
  syd::Tag::vector tags;
  db->FindTags(tags, tagname);

  // Get the list of images
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=1; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::Image::vector images;
  db->Query(images, ids);
  if (images.size() == 0) {
    LOG(FATAL) << "No image ids given. I do nothing.";
  }

  // We only consider the calibration of the first image
  syd::Calibration::pointer calib;
  typedef odb::query<syd::Calibration> QC;
  QC q = QC::image == images[0]->id;
  try {
      db->QueryOne(calib, q);
  } catch(std::exception & e) {
    LOG(FATAL) << "Error while searching calibration for this image: " << images[0]
               << "." << std::endl << e.what();
  }
  LOG(2) << "Using calibration factor: " << calib->factor << " (id= " << calib->id << ")";

  // Create main builder
  syd::DecayCorrectedImageBuilder builder(db);
  syd::Image::vector result_images;
  for(auto image:images) {
    syd::Image::pointer result = builder.CreateDecayCorrectedImage(image, calib);
    for(auto t:tags) result->AddTag(t);
    result_images.push_back(result);
    LOG(1) << "Inserting Image " << result;
  }

  // Update for tag
  db->Update(result_images);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
