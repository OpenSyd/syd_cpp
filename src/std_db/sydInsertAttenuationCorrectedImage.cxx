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
#include "sydInsertAttenuationCorrectedImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydCommonGengetopt.h"
#include "sydAttenuationCorrectedImage.h"
#include <numeric>

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertAttenuationCorrectedImage, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the Geometrical Mean (GM) image id
  syd::IdType id_GM = atoi(args_info.inputs[0]);
  syd::Image::pointer input_GM;
  db->QueryOne(input_GM, id_GM); // will fail if not found
  LOG(2) << "Read geometrical mean image :" << input_GM;

  // Get the projected factor attenuation map (AM) id
  syd::IdType id_AM = atoi(args_info.inputs[1]);
  syd::Image::pointer input_AM;
  db->QueryOne(input_AM, id_AM); // will fail if not found
  LOG(2) << "Read projected factor attenuation map :" << input_AM;

  // Main computation
  auto image = syd::InsertAttenuationCorrectedImage(input_GM, input_AM);

  // Update image info
  syd::SetImageInfoFromImage(image, input_GM);
  syd::SetTagsFromCommandLine(image->tags, db, args_info);
  syd::SetImageInfoFromCommandLine(image, args_info);
  syd::SetCommentsFromCommandLine(image->comments, db, args_info);
  db->Update(image);
  LOG(1) << "Inserting Image " << image;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
