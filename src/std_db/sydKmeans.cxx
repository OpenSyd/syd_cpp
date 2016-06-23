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
#include "kmeans/KMeans.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydKmeans, 4);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();

  // Get the database
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Input points
  std::string points_filename = args_info.inputs[0];
  auto points = syd::NDimPoints::New();
  points->Load(points_filename);
  DD(points_filename);

  // typedef
  typedef syd::KmeansFilter::ImageType ImageType;
  typedef syd::KmeansFilter::Image4DType Image4DType;

  // Input mask
  std::string mask_filename = args_info.inputs[1];
  auto mask = syd::ReadImage<ImageType>(mask_filename);

  // Input image
  std::string image_filename = args_info.inputs[2];
  auto input_image = syd::ReadImage<Image4DType>(image_filename);

  int N = points->size();
  int D = points->GetNumberOfDimensions();
  int K = atoi(args_info.inputs[3]);
  DD(N);
  DD(D);
  DD(K);

  if (1) {
    // Trial kmeans
    syd::KmeansFilter filter;
    filter.SetInput(points);
    filter.SetNumberOfClusters(K);
    filter.Run();

    // Compute image
    auto centers = filter.GetCenters();
    auto output_image = filter.ComputeLabeledImage(centers, mask, input_image);
    syd::WriteImage<ImageType>(output_image, args_info.output_arg);
  }

  // Alternative implementation
  Scalar * pts = new Scalar[N*D];//points->begin()[0];
  for(auto i=0; i<N; i++) {
    for(auto j=0; j<D; j++) {
      pts[D*i+j] = *(points->begin()+i)[j];
    }
  }
  for(auto i=0; i<10; i++) {
    std::cout << pts[i] << " ";
  }
  std::cout << std::endl;
  int attempts = 5;
  Scalar * centers = new Scalar[D*K];
  int * assignments = 0; // no assignments
  Scalar result = RunKMeansPlusPlus(N, K, D, pts, attempts, centers, assignments);
  DD(result);
  for(auto i=0; i<K; i++) {
    for(auto j=0; j<D; j++) {
      std::cout << centers[D*i+j] << " ";
    }
    std::cout << std::endl;
  }

  // Compute image
  // auto output_image = filter.ComputeLabeledImage(centers, mask, input_image);
  // syd::WriteImage<ImageType>(output_image, args_info.output_arg+"_2.mhd");


  // This is the end, my friend.
}
// --------------------------------------------------------------------
