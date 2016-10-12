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
#include "sydInsertRoiMaskImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydRoiMaskImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydCommonGengetopt.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertRoiMaskImage, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db =
    m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the roitype
  syd::RoiType::pointer roitype =
    syd::FindRoiType(args_info.inputs[0], db);

  // Get the image
  syd::IdType id = atoi(args_info.inputs[1]);
  syd::Image::pointer image;
  db->QueryOne(image, id);

  // Get mask filename
  std::string filename = args_info.inputs[2];

  // Create a new RoiMaskImage
  auto mask = syd::InsertRoiMaskImageFromFile(filename, image->patient, roitype);

  // Copy information from the iamge
  mask->frame_of_reference_uid = image->frame_of_reference_uid;
  mask->CopyDicomSeries(image);
  mask->acquisition_date = image->acquisition_date;
  syd::AddTag(mask->tags, image->tags);
  syd::SetImageInfoFromCommandLine(mask, args_info);
  syd::SetTagsFromCommandLine(mask->tags, db, args_info);
  syd::SetCommentsFromCommandLine(mask->comments, db, args_info);
  db->Update(mask);
  LOG(1) << "Inserting RoiMaskImage " << mask;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
