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
#include "sydInsertChangAttenuationImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydCommonGengetopt.h"
#include <numeric>

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertChangAttenuationImage, 1);

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

  //Get the number of Angles
  int nbAngles = args_info.angles_arg;
  if (nbAngles <= 0) {
    LOG(2) << "Wrong number of angles";
    return -1;
  }

  auto changImage = syd::InsertChangAttenuation(inputImage, nbAngles);

  // set properties from the image
  syd::SetImageInfoFromImage(changImage, inputImage);

  // Update image info
  syd::SetTagsFromCommandLine(changImage->tags, db, args_info);
  syd::SetImageInfoFromCommandLine(changImage, args_info);
  syd::SetCommentsFromCommandLine(changImage->comments, db, args_info);
  db->Update(changImage);
  LOG(1) << "Inserting Image " << changImage;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
