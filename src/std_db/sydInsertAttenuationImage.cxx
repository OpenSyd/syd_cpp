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
#include "sydInsertAttenuationImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydCommonGengetopt.h"
#include "sydAttenuationImage.h"
#include <numeric>

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertAttenuationImage, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of images id
  syd::IdType id = atoi(args_info.inputs[0]);
  syd::Image::pointer input;
  db->QueryOne(input, id); // will fail if not found
  LOG(2) << "Read image :" << input;

  //Verify inputs
  if (args_info.attenuationCT_given != 2)
    LOG(FATAL) << "The numbers of attenuationCT inputs is not 2.";

  double numberEnergySPECT(1);
  if (args_info.weight_given)
    numberEnergySPECT = args_info.weight_given;
  if (3*numberEnergySPECT != args_info.attenuationSPECT_given)
    LOG(FATAL) << "The numbers of weight and attenuationSPECT inputs do not match.";

  double attenuationWaterCT(0);
  double attenuationBoneCT(0);
  attenuationWaterCT = args_info.attenuationCT_arg[0];
  attenuationBoneCT = args_info.attenuationCT_arg[1];
  if (attenuationWaterCT > attenuationBoneCT)
    LOG(FATAL) << "Attenuation water coefficient > Attenuation bone coefficient (check the order of attenuationCT)";

  std::vector<double> attenuationAirSPECT, attenuationWaterSPECT, attenuationBoneSPECT;
  for(unsigned int i=0; i<numberEnergySPECT; ++i) {
    if (args_info.attenuationSPECT_arg[3*i] <= args_info.attenuationSPECT_arg[3*i+1] && args_info.attenuationSPECT_arg[3*i+1] <= args_info.attenuationSPECT_arg[3*i+2]) {
      attenuationAirSPECT.push_back(args_info.attenuationSPECT_arg[3*i]);
      attenuationWaterSPECT.push_back(args_info.attenuationSPECT_arg[3*i+1]);
      attenuationBoneSPECT.push_back(args_info.attenuationSPECT_arg[3*i+2]);
    }
    else {
      LOG(FATAL) << "Attenuation air coefficient > Attenuation water coefficient > Attenuation bone coefficient (check the order of attenuationSPECT)";
      break;
    }
  }

  std::vector<double> weight;
  if (args_info.weight_given) {
    for(unsigned int i=0; i<numberEnergySPECT; ++i)
      weight.push_back(args_info.weight_arg[i]);
  }
  else
    weight.push_back(1.0);

  if (std::abs(std::accumulate(weight.begin(), weight.end(), 0.0)-1.0) > 0.000001)
    LOG(FATAL) << "The sum of weights is not equal to 1.0";

  // Main computation
  auto image = syd::InsertAttenuationImage(input, numberEnergySPECT, attenuationWaterCT, attenuationBoneCT, attenuationAirSPECT, attenuationWaterSPECT, attenuationBoneSPECT, weight);

  // Update image info
  syd::SetTagsFromCommandLine(image->tags, db, args_info);
  syd::SetImageInfoFromCommandLine(image, args_info);
  syd::SetCommentsFromCommandLine(image->comments, db, args_info);
  db->Update(image);
  LOG(1) << "Inserting Image " << image;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
