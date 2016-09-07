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
#include "sydUpdateDoseImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydRoiStatisticBuilder.h"
#include "sydCommonGengetopt.h"
#include "sydScaleImageBuilder.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydUpdateDoseImage, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the tia image
  syd::IdType id = atoi(args_info.inputs[0]);
  syd::Image::pointer tia;
  db->QueryOne(tia, id);
  LOG(2) << "TIA image is " << tia;

  // Get N the nb of primaries
  double N = atof(args_info.inputs[1]);
  LOG(2) << "Nb of primaries is: " << N;

  // Get the image
  id = atoi(args_info.inputs[2]);
  syd::Image::pointer image;
  db->QueryOne(image, id);
  LOG(2) << "Image to update is " << image;

  // Get the optional scaling factor
  double s = 1.0;
  if (args_info.scale_given) s = args_info.scale_arg;

  // Compute the total nb of counts in the tia image
  syd::RoiStatisticBuilder rsbuilder(db);
  syd::RoiStatistic::pointer stat;
  db->New(stat);
  stat->image = tia;
  rsbuilder.ComputeStatistic(stat);// no mask
  s = s * (stat->sum / N);
  LOG(2) << "Total counts is  " << stat->sum;
  LOG(2) << "Final scaling is " << s;
  syd::ScaleImageBuilder builder(db);
  builder.Scale(image, s);

  // If needed update the unit
  if (args_info.pixelunit_given) builder.SetImagePixelValueUnit(image, args_info.pixelunit_arg);

  // Set the tag if needed
  db->UpdateTagsFromCommandLine(image->tags, args_info);

  // Finally update the image in the db
  db->Update(image);
  LOG(1) << "Image was scaled by " << s << ": " << image;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
