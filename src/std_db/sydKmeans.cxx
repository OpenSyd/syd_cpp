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
#include "sydKmeans_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"
#include "sydKmeansInputDataBuilder.h"
#include "sydKmeansFilter.h"
#include "sydHistogram.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydKmeans, 4);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Input points
  std::string points_filename = args_info.inputs[0];
  auto points = syd::NDimPoints::New();
  points->Load(points_filename);

  // typedef
  typedef syd::KmeansFilter::ImageType ImageType;
  typedef syd::KmeansFilter::Image4DType Image4DType;

  // Input mask
  std::string mask_filename = args_info.inputs[1];
  auto mask = syd::ReadImage<ImageType>(mask_filename);

  // Input image
  std::string image_filename = args_info.inputs[2];
  auto input_image = syd::ReadImage<Image4DType>(image_filename);

  // Trial kmeans
  int K = atoi(args_info.inputs[3]);
  syd::KmeansFilter filter;
  filter.SetInput(points);
  filter.SetNumberOfClusters(K);
  filter.Run();

  // Compute image
  auto centers = filter.GetCenters();
  auto output_image = filter.ComputeLabeledImage(centers, mask, input_image);
  syd::WriteImage<ImageType>(output_image, args_info.output_arg);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
