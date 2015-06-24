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
#include "syd_elastix_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydTableImageTransform.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT(syd_elastix, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the images
  syd::Image fixed_image;
  db->QueryOne(fixed_image, atoi(args_info.inputs[1]));
  syd::Image moving_image;
  db->QueryOne(moving_image, atoi(args_info.inputs[2]));
  DD(fixed_image);
  DD(moving_image);

  // Get the masks
  //FIXME later

  // Get the elastix config file
  std::string config_file = args_info.inputs[3];
  DD(config_file);

  // Get elastix param
  std::string options = args_info.options_arg;
  DD(options);

  // Create result record (must be inserted before computation to get the id for the filename)
  syd::ImageTransform transfo;
  syd::InsertNewImageTransform(transfo, db, fixed_image, moving_image, config_file);
  //  syd::Execute_Elastix(transfo, fixed_image, moving_image, config_file, parameters); // no mask yet
  DD(transfo);

  // Update in the db
  db->Update(transfo);
  LOG(1) << "Registration computed. Result: " << transfo;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
