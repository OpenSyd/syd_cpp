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
#include "sydInsertPlanarGeometricalMean_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageHelper.h"
#include "sydCommonGengetopt.h"
#include "sydImageExtractSlices.h"
#include "sydImageGeometricalMean.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertPlanarGeometricalMean, 1);

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
  auto filename = db->GetAbsolutePath(input);
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  auto itk_input = syd::ReadImage<ImageType>(filename);
  double k = args_info.k_arg;

  // Check only 4 slices
  int n = itk_input->GetLargestPossibleRegion().GetSize()[2];
  if (n != 4) {
    LOG(FATAL) << "Error I expect 4 slices only :  ANT_EM POST_EM ANT_SC POST_SC";
  }

  // Get the 4 images ANT_EM POST_EM ANT_SC POST_SC
  std::vector<ImageType::Pointer> itk_images;
  syd::ExtractSlices<ImageType>(itk_input, 2, itk_images); // Direction = Z (2)
  auto ant_em = itk_images[0];
  auto post_em = itk_images[1];
  auto ant_sc = itk_images[2];
  auto post_sc = itk_images[3];
  auto gmean = syd::GeometricalMean<ImageType>(ant_em, post_em, ant_sc, post_sc, k);
  auto image = syd::InsertImage<ImageType>(gmean, input->patient, input->modality);

  // Update image info
  db->UpdateTagsFromCommandLine(image->tags, args_info);
  syd::SetImageInfoFromCommandLine(image, args_info);
  db->Update(image);
  LOG(1) << "Inserting Image " << image;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
