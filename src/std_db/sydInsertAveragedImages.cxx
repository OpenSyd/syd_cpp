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
#include "sydInsertAveragedImages_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydPixelUnitHelper.h"
#include "sydInjectionHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertAveragedImages, 0);

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

  // Get the images
  syd::Image::vector images;
  db->Query(images, ids);

  // Average
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  std::vector<std::string> filenames;
  for(auto image:images) {
    LOG(2) << "Reading image: " << image;
    filenames.push_back(image->GetAbsolutePath());
  }
  auto itk_image = syd::ComputeAverageImage<ImageType>(filenames);

  // Create new image
  auto output = syd::InsertImage<ImageType>(itk_image, images[0]->patient);
  syd::SetImageInfoFromImage(output, images[0]);
  syd::SetImageInfoFromCommandLine(output, args_info);
  syd::SetTagsFromCommandLine(output->tags, db, args_info);
  db->Update(output);
  LOG(1) << "Image was created: " << output;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
