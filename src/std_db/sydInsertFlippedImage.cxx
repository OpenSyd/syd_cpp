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
#include "sydInsertFlippedImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydCommonGengetopt.h"
#include "sydManualRegistration.h"
#include <numeric>

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertFlippedImage, 1);

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

  //Get the axis
  std::string axis("");
  if (args_info.axis_given)
    axis = args_info.axis_arg;
  syd::Image::pointer image;

  bool flipOrigin(false);
  if (args_info.origin_flag)
    flipOrigin = true;
  
  image = inputImage;
  std::vector<char> axisChar;
  for (auto iter=axis.begin(); iter != axis.end(); ++iter) {
    if (*iter == 'x')
      axisChar.push_back(0);
    else if  (*iter == 'y')
      axisChar.push_back(1);
    else if (*iter == 'z')
      axisChar.push_back(2);
    else {
      LOG(2) << "Wrong axis";
      return -1;
    }
  }
  auto flippedImage = syd::InsertFlip(image, axisChar, flipOrigin);

  // set properties from the image
  syd::SetImageInfoFromImage(flippedImage, inputImage);

  // Update image info
  syd::SetTagsFromCommandLine(flippedImage->tags, db, args_info);
  syd::SetImageInfoFromCommandLine(flippedImage, args_info);
  syd::SetCommentsFromCommandLine(flippedImage->comments, db, args_info);
  db->Update(flippedImage);
  LOG(1) << "Inserting Image " << flippedImage;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
