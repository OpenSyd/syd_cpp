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
#include "sydStandardDatabase.h"
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
  syd::IdType id = atoi(args_info.inputs[1]);
  syd::Image::pointer image;
  db->QueryOne(image, id);

  // Get the mask
  syd::RoiStatisticBuilder builder(db);
  syd::RoiMaskImage::pointer mask = builder.FindMask(image, args_info.inputs[0]);

  // Init the RoiStatistic object
  bool newStat = false;
  syd::RoiStatistic::pointer stat;
  if (!builder.Exists(&stat, image, mask)) {
    db->New(stat);
    stat->image = image;
    stat->mask = mask;
    newStat = true;
  }

  // Update the value
  builder.ComputeStatistic(stat);

  // Update
  if (newStat) {
    db->Insert(stat);
    LOG(1) << "Insert RoiStatistic: " << stat;
  }
  else {
    db->Update(stat);
    LOG(1) << "Update RoiStatistic: " << stat;
  }
}
// --------------------------------------------------------------------
