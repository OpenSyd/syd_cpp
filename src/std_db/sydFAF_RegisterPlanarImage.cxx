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
#include "sydFAF_RegisterPlanarImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydCommonGengetopt.h"
#include "sydFAFHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydFAF_RegisterPlanarImage, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of images id (planar, projected spect and projected attenuation map)
  syd::IdType idPlanar = atoi(args_info.inputs[0]);
  syd::Image::pointer inputPlanar;
  db->QueryOne(inputPlanar, idPlanar); // will fail if not found
  LOG(2) << "Read image :" << inputPlanar;

  syd::IdType idSPECT = atoi(args_info.inputs[1]);
  syd::Image::pointer inputSPECT;
  db->QueryOne(inputSPECT, idSPECT); // will fail if not found
  LOG(2) << "Read image :" << inputSPECT;

  // Main computation
  double t = 0.0;
  syd::ImageProjection_Parameters p;
  p.projectionDimension = args_info.dimension_arg;
  p.flipProjectionFlag = args_info.flip_flag;
  p.meanFlag = false;
  auto image = syd::InsertRegisterPlanarSPECT(inputPlanar, inputSPECT, p, t, args_info.debug_output_arg);
  LOG(1) << "Translation Sup-Inf (Y) is " << t << " mm.";

  // Update image info
  syd::SetTagsFromCommandLine(image->tags, db, args_info);
  syd::SetImageInfoFromCommandLine(image, args_info);
  syd::SetCommentsFromCommandLine(image->comments, db, args_info);
  db->Update(image);
  LOG(1) << "Insert new image " << image;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
