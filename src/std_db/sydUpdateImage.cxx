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
#include "sydUpdateImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydRoiStatisticBuilder.h"
#include "sydCommonGengetopt.h"
#include "sydScaleImageBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydUpdateImage, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the ids
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=0; i<args_info.inputs_num; i++)
    ids.push_back(atoi(args_info.inputs[i]));

  // Get the images to udpate
  syd::Image::vector images;
  db->Query(images, ids);

  // Check & updates the images
  for(auto image:images) {
    // check file exist
    bool b = true;
    for(auto file:image->files) {
      std::string s = file->GetAbsolutePath(db);
      if (!fs::exists(s)) {
        LOG(WARNING) << "Image: " << image << std::endl
                     << "--> the file '" << s << "' does not exist." << std::endl;
        b = false;
      }
    }
    if (b) {

      // Create the update builder
      syd::ScaleImageBuilder builder(db);
      double s = 1.0;

      // Need to scale ?
      if (args_info.scale_given) {
        s = args_info.scale_arg;
        if (args_info.N_given and args_info.tia_given) {
          syd::IdType id = args_info.tia_arg;
          syd::Image::pointer tia;
          db->QueryOne(tia, id);
          syd::RoiStatisticBuilder builder(db);
          builder.SetImage(tia); // no mask, whole image
          syd::RoiStatistic::pointer stat = builder.ComputeStatistic();
          s = s * (stat->sum / args_info.N_arg);
        }
        else {
          if (args_info.N_given or args_info.tia_given) {
            LOG(WARNING) << "Option -N and --tia must be both set. Ignoring. ";
          }
        }
        // scale and update
        builder.ScalePixelValue(image, s);
      }

      // If needed update the unit
      if (args_info.pixelunit_given) {
        syd::PixelValueUnit::pointer unit;
        odb::query<syd::PixelValueUnit> q = odb::query<syd::PixelValueUnit>::name == args_info.pixelunit_arg;
        try {
          db->QueryOne(unit, q);
          image->pixel_value_unit = unit;
        } catch(std::exception & e) {
          LOG(WARNING) << "Cannot find the unit '" << args_info.pixelunit_arg << "', ignoring.";
        }
      }

      // update db
      db->Update(image);
      LOG(1) << "Image was scaled by " << s << ": " << image;
    }
  }
  // This is the end, my friend.
}
// --------------------------------------------------------------------
