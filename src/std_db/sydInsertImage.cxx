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
#include "sydInsertImage_ggo.h"
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
  SYD_INIT_GGO(sydInsertImage, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the mhd filename
  std::string filename = args_info.inputs[0];

  // Get the image to copy info from
  if (!args_info.like_given) {
    LOG(FATAL) << "Error flag 'like' is mandatory (yet).";
  }
  syd::IdType id = args_info.like_arg;
  syd::Image::pointer input;
  db->QueryOne(input, id);

  // Create the new image
  syd::Image::pointer output;
  db->New(output);
  output = input; // copy the fields
  output->id = -1;
  db->Insert(output);

  // Update image data (size etc) from file
  db->UpdateImageInfoFromFile(output, filename);

  // Copy the file in the db
  std::string absolutepath = output->ComputeDefaultAbsolutePath(db);
  syd::CopyMHDImage(filename, absolutepath);
  output->UpdateFile(db, absolutepath);

  // Update the db
  db->Update(output);
  LOG(1) << "Inserting Image " << output;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
