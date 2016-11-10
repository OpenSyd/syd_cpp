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
#include "sydInsertAttenuationCorrectedProjectionImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommonGengetopt.h"
#include "sydAttenuationCorrectedProjectionImage.h"
#include <numeric>

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertAttenuationCorrectedProjectionImage, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the Geometrical Mean (GM) image id
  syd::IdType id_GM;
  if (args_info.image1_given)
    id_GM = args_info.image1_arg;
  syd::Image::pointer input_GM;
  db->QueryOne(input_GM, id_GM); // will fail if not found
  LOG(2) << "Read geometrical mean image :" << input_GM;

  // Get the attenuation map (AM) id
  syd::IdType id_AM;
  if (args_info.image2_given)
    id_AM = args_info.image2_arg;
  syd::Image::pointer input_AM;
  db->QueryOne(input_AM, id_AM); // will fail if not found
  LOG(2) << "Read attenuation map :" << input_AM;

  //Read the projection dimension
  int dimension(0);
  if (args_info.dimension_given)
    dimension = args_info.dimension_arg;

  // Main computation
  auto image = syd::InsertAttenuationCorrectedProjectionImage(input_GM, input_AM, dimension);

  // Update image info
  syd::SetTagsFromCommandLine(image->tags, db, args_info);
  syd::SetImageInfoFromCommandLine(image, args_info);
  db->Update(image);
  LOG(1) << "Inserting Image " << image;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
