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
#include "sydSubstituteRadionuclide_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydPixelUnitHelper.h"
#include "sydRadionuclideHelper.h"
#include "sydInjectionHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydSubstituteRadionuclide, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the ids
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=1; i<args_info.inputs_num; i++)
    ids.push_back(atoi(args_info.inputs[i]));

  // Get the radionuclide
  auto rad = syd::FindRadionuclide(db, args_info.inputs[0]);

  // Get the images to udpate
  syd::Image::vector images;
  syd::Injection::pointer inj;
  db->Query(images, ids);

  // Loop over the images
  for(auto image:images) {
    // Make a copy
    auto output = syd::InsertCopyImage(image);

    // Get/create injection
    auto new_injection = syd::CopyInjection(image->injection);
    new_injection->radionuclide = rad;
    //    new_injection->activity_in_MBq = 1.0;
    auto inj = syd::GetSimilarInjection(db, new_injection);
    if (inj.size() != 0) {
      LOG(2) << "Similar injection exist, do not add " << std::endl
             << new_injection << std::endl
             << inj[0];
      new_injection = inj[0];
    }
    else db->Insert(new_injection);

    // Change the radionuclide
    syd::SubstituteRadionuclide(output, new_injection);

    // Apply user information
    syd::SetImageInfoFromCommandLine(output, args_info);
    syd::SetTagsFromCommandLine(output->tags, db, args_info);
    db->Update(output);
    LOG(1) << "Image with new radionuclide is " << output << ": "
           << std::endl << "Injection : " << new_injection;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
