/*=========================================================================
  Program:   sfz

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
#include "sydInsertGateOutput_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"
#include "sydGateHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydInsertGateOutput, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the folder
  std::string folder_name = args_info.inputs[0];

  // Get the injection
  syd::IdType id = atoi(args_info.inputs[1]);
  auto source = db->QueryOne<syd::Image>(id);
  DD(source);

  // Get the images (dose+edep+uncert)
  auto images = syd::GateInsertOutputImages(folder_name, source);
  DDS(images);

  // Scaling ?

  //  auto stat_file = syd::GateGetStatFile(folder_name);
  //DD(stat_file);

  // Scaling process FIXME


  // Tags from cmd line
  //  db->UpdateTagsFromCommandLine(output->tags, args_info); // user defined tags


  // This is the end, my friend.
}
// --------------------------------------------------------------------
