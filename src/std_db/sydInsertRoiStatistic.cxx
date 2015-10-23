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
#include "sydInsertRoiStatistic_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydRoiStatisticBuilder.h"
#include "sydCommonGengetopt.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertRoiStatistic, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the Image
  std::string id = atoi(args_info.inputs[1]);
  syd::Image::pointer image;
  db->QueryOne(image, id);
  DD(image);

  // Get the RoiMaskImage
  std::string roiname = args_info.inputs[0]; // FIXME or by id
  syd::RoiMaskImage::pointer roimask;
  //  db->FindRoiMaskImage(roimask, image, roi); // FIXME consider a single function for all that part
  syd::RoiType::vector roitype = db->FindRoiType(roiname);
  DD(roitype);
  odb::query<syd::RoiMaskImage> q = odb::query<RoiMaskImage>::roitype = roitype->id and
    odb::query<RoiMaskImage>::frame_of_reference_uid == image->frame_of_reference_uid;
  syd::RoiMaskImage::vector roimasks;
  db->Query(roimasks, q);
  if (roimasks.size() == 0) {
    LOG(FATAL) << "Cannot find a roimask of type '" << roitype->name << "' with same frame_of_reference_uid for the image: " image;
  }
  if (roimasks.size() > 1) {
    LOG(FATAL) << "Several roimask exist with type '" << roitype->name << "' and same frame_of_reference_uid for the image: " image;
  }
  roimask = roimasks[0];
  DD(roimask);

  // FIXME what about the number of value in the mask ?
  // should be in RoiMaskImage ?
  // int nb_pixels;
  // double volume;

  // If image is a ct, mean = mean HU, could be converted into density ? --> to include into RoiMaskImage ?
  // sydUpdateRoiMaskImage with ct ? bof.
  // no keep as a RoiStatistic special

  // FIXME resampling mask <-> image ?
  // FIXME what about count !? warning.



  // Create a RoiStatistic
  syd::RoiStatistic::pointer stat;
  db->New(stat);
  stat->image = image;
  stat->roi_mask_image = roimask;
  stat->mean = mean; //etc

  DD(stat);
  //  db->Update(stat);


  // This is the end, my friend.
}
// --------------------------------------------------------------------
