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
#include "sydFAF_Calibration_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageHelper.h"
#include "sydFAFHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydCommonGengetopt.h"
#include "sydFAFHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydFAF_Calibration, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the SPECT image id
  syd::IdType id_SPECT = atoi(args_info.inputs[0]);
  syd::Image::pointer input_SPECT;
  db->QueryOne(input_SPECT, id_SPECT); // will fail if not found
  LOG(2) << "Read SPECT image :" << input_SPECT;

  // Get the ACGM image id
  syd::IdType id_planar = atoi(args_info.inputs[1]);
  syd::Image::pointer input_planar;
  db->QueryOne(input_planar, id_planar); // will fail if not found
  LOG(2) << "Read ACGM planar image :" << input_planar;

  // Main computation
  syd::ImageProjection_Parameters p;
  p.projectionDimension = args_info.dim_arg;
  p.flipProjectionFlag = args_info.flip_flag;
  p.meanFlag = false;
  auto image = syd::InsertFAFCalibratedImage(input_SPECT, input_planar, args_info.nb_heads_arg, p);
  DD(image);

  // Update image info
  syd::SetTagsFromCommandLine(image->tags, db, args_info);
  syd::SetImageInfoFromCommandLine(image, args_info);
  syd::SetCommentsFromCommandLine(image->comments, db, args_info);
  db->Update(image);
  DD(image);
  LOG(1) << "Inserting Image " << image;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
