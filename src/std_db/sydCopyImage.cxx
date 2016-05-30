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
#include "sydCopyImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageFromDicomBuilder.h"
#include "sydCommonGengetopt.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydCopyImage, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of images to copy
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=0; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::Image::vector images;
  db->Query(images, ids);

  // Copy the images
  syd::ImageBuilder builder(db);
  for(auto image:images) {
    // Copy the information
    syd::Image::pointer output = builder.NewMHDImageLike(image);
    db->UpdateTagsFromCommandLine(output->tags, args_info);
    // Copy the file
    std::string filename = syd::CreateTemporaryFile(db->GetDatabaseAbsoluteFolder(), ".mhd");
    syd::CopyMHDImage(db->GetAbsolutePath(image), filename);
    builder.CopyImageFromFile(output, filename);
    builder.InsertAndRename(output);
    fs::remove(filename);
    LOG(1) << "Insert image: " << output;
  }
  // This is the end, my friend.
}
// --------------------------------------------------------------------
