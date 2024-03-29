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
#include "sydInsertFAFMask_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydRoiMaskImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydCommonGengetopt.h"
#include "sydFAFMask.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertFAFMask, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the SPECT image id
  syd::IdType id_SPECT = atoi(args_info.inputs[0]);
  syd::Image::pointer input_SPECT;
  db->QueryOne(input_SPECT, id_SPECT); // will fail if not found
  LOG(2) << "Read SPECT image :" << input_SPECT;

  // Get the AC planar image id
  syd::IdType id_planar = atoi(args_info.inputs[1]);
  syd::Image::pointer input_planar;
  db->QueryOne(input_planar, id_planar); // will fail if not found
  LOG(2) << "Read attenuation corrected planar image :" << input_planar;

  // Main computation
  auto image = syd::InsertFAFMask(input_SPECT, input_planar);

  // Update image info
  syd::SetImageInfoFromImage(image, input_planar);
  syd::SetTagsFromCommandLine(image->tags, db, args_info);
  syd::SetImageInfoFromCommandLine(image, args_info);
  syd::SetCommentsFromCommandLine(image->comments, db, args_info);
  db->Update(image);
  LOG(1) << "Inserting Image " << image;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
