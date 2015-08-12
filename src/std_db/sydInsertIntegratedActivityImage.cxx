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
#include "sydInsertIntegratedActivityImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydIntegratedActivityImageBuilder.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  SYD_CERES_STATIC_INIT;

  // Init
  SYD_INIT_GGO(sydInsertIntegratedActivityImage, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the tag
  std::string tagname = args_info.inputs[0];
  syd::Tag::vector tags;
  db->FindTags(tags, tagname);

  // Get the list of images to integrate
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=1; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::Image::vector images;
  db->Query(images, ids);
  DDS(images);

  // Sort by time
  std::sort(begin(images), end(images),
            [images](syd::Image::pointer a, syd::Image::pointer b) {
              if (a->dicoms.size() == 0) return true;
              if (b->dicoms.size() == 0) return false;
              return a->dicoms[0]->acquisition_date < b->dicoms[0]->acquisition_date;
            });

  // FIXME
  DD("FIXME : make some check on images list here");

  // image type
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;

  // Create main builder
  syd::IntegratedActivityImageBuilder builder;

  // Read the images+times and set to the builder
  std::string starting_date = images[0]->dicoms[0]->injection->date;
  for(auto image:images) {
    auto im = syd::ReadImage<ImageType>(db->GetAbsolutePath(image));
    double t = syd::DateDifferenceInHours(image->dicoms[0]->acquisition_date, starting_date);
    builder.AddInput(im, t);
  }


  builder.debug_only_flag_ = args_info.only_debug_flag;
  builder.robust_scaling_ = args_info.robust_scaling_arg;
  builder.gauss_sigma_ = args_info.gauss_arg;
  builder.activity_threshold_ = args_info.min_activity_arg;

  if (args_info.debug_given) {
    std::string file=args_info.debug_arg;
    std::ifstream is(file);
    while (is) {
      std::string name;
      int x,y,z;
      is >> name >> x >> y >> z;
      if (is) {
        if (name[0] != '#') builder.AddDebugPixel(name, x,y,z);
      }
    }
  }

  // Go !
  builder.CreateIntegratedActivityImage();

  // Debug here //FIXME
  builder.SaveDebugPixel("gp/tac.txt");
  builder.SaveDebugModel("gp/models.txt");

  // Output
  DD("FIXME : insert builder output in the db");

  // Update tags
  /*
  for(auto t:tags) image->AddTag(t);
  db->Update(image);
  LOG(1) << "Inserting Image " << image;
  */

  DD("done");
  // This is the end, my friend.
}
// --------------------------------------------------------------------
