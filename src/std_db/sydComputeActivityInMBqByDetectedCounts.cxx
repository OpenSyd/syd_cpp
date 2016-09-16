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
#include "sydComputeActivityInMBqByDetectedCounts_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"
#include "sydImageHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydComputeActivityInMBqByDetectedCounts, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the images
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=0; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::Image::vector images;
  db->Query(images, ids);
  if (images.size() == 0) {
    LOG(FATAL) << "No image found. I do nothing.";
  }

  // Loop on images
  for(auto image:images) {
    DD(image);
    double s = syd::ComputeActivityInMBqByDetectedCounts(image);
    DD(s);
    DD(s*1000000);
    s = s * args_info.scale_arg;
    // print result:
    // Verbose = 0  -> as a single line
    // Verbose >= 1 -> several lines
    DD(sydlog::Log::LogLevel());
    auto l = sydlog::Log::LogLevel();
    if (l == 0) {
      std::cout << s << " ";
    }
    else {
      LOG(1) << image->id << " "  << s;
    }
  }
  if (sydlog::Log::LogLevel() == 0) std::cout << std::endl;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
