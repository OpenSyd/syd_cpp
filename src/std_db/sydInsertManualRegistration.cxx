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
#include "sydInsertManualRegistration_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommonGengetopt.h"
#include "sydManualRegistration.h"
#include <numeric>

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertManualRegistration, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of images id
  syd::IdType idImage = atoi(args_info.inputs[0]);
  syd::Image::pointer inputImage;
  db->QueryOne(inputImage, idImage); // will fail if not found
  LOG(2) << "Read image :" << inputImage;

  //Get x, y, z displacement
  double x(0), y(0), z(0);
  bool translateOrigin(0);
  if (args_info.x_given)
    x = args_info.x_arg;
  if (args_info.y_given)
    y = args_info.y_arg;
  if (args_info.z_given)
    z = args_info.z_arg;
  if (args_info.origin_given)
    translateOrigin = args_info.origin_flag;

  // Main computation
  auto image = syd::InsertManualRegistration(inputImage, x, y, z, translateOrigin);

  // Update image info
  syd::SetTagsFromCommandLine(image->tags, db, args_info);
  syd::SetImageInfoFromCommandLine(image, args_info);
  db->Update(image);
  LOG(1) << "Inserting Image " << image;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
