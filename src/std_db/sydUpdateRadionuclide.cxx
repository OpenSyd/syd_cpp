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
#include "sydUpdateRadionuclide_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydUpdateRadionuclideFilter.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydUpdateRadionuclide, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the radionuclide name
  std::string radname = args_info.inputs[0];
  std::vector<std::string> radnames;
  if (radname == "all") {
    syd::Radionuclide::vector rad;
    db->Query(rad);
    for(auto r:rad) radnames.push_back(r->name);
  }
  else {
    for(auto i=0; i<args_info.inputs_num; i++)
      radnames.push_back(args_info.inputs[i]);
  }

  syd::UpdateRadionuclideFilter filter(db);
  filter.SetURL(args_info.url_arg, args_info.path_arg);
  auto rads = filter.Update(radnames);
  for(auto r:rads)
    LOG(1) << "Update " << r;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
