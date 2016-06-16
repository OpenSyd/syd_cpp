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
#include "sydKmeansBuildInputData_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"
#include "sydKmeansInputDataBuilder.h"
#include "sydHistogram.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydKmeansBuildInputData, 4);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Input mask image
  syd::IdType id = atoi(args_info.inputs[0]);
  syd::Image::pointer mask;
  db->QueryOne(mask, id);

  // Input tia image
  id = atoi(args_info.inputs[1]);
  syd::Image::pointer tia;
  db->QueryOne(tia, id);

  // Input model
  id = atoi(args_info.inputs[2]);
  syd::Image::pointer model;
  db->QueryOne(model, id);

  // Input params (4D image)
  id = atoi(args_info.inputs[3]);
  syd::Image::pointer params;
  db->QueryOne(params, id);

  syd::Image::vector images;
  for(auto i=4; i<args_info.inputs_num; i++) {
    DD(i);
    id = atoi(args_info.inputs[3]);
    syd::Image::pointer im;
    db->QueryOne(im, id);
    images.push_back(im);
  }

  // list of features
  std::vector<int> features;
  for(auto i=0; i<args_info.features_given; i++)
    features.push_back(args_info.features_arg[i]);
  if (features.size() == 0) {
    LOG(FATAL) << "Provide at least one feature index";
  }

  // Main builder
  syd::KmeansInputDataBuilder builder;
  typedef syd::KmeansInputDataBuilder::ImageType ImageType;
  typedef syd::KmeansInputDataBuilder::Image4DType Image4DType;
  builder.SetMask(syd::ReadImage<ImageType>(db->GetAbsolutePath(mask)));
  std::vector<int> indices;
  for(auto f:features) {
    if (f==0) builder.AddInput(syd::ReadImage<ImageType>(db->GetAbsolutePath(tia)));
    if (f==1) builder.AddInput(syd::ReadImage<ImageType>(db->GetAbsolutePath(model)));
    // consider the index for the params images
    if (f==2) indices.push_back(0);
    if (f==3) indices.push_back(1);
    if (f==4) indices.push_back(2);
    if (f==5) indices.push_back(3);
    if (f>5) {
      DD(f-6);
      builder.AddInput(syd::ReadImage<ImageType>(db->GetAbsolutePath(images[f-6])));
    }
  }
  builder.AddInput(syd::ReadImage<Image4DType>(db->GetAbsolutePath(params)), indices);

  // FIXME gauss
  // FIXME normalize

  // Go
  builder.BuildInputData();

  // Get output
  auto & points = builder.GetInputKmeansData();
  int nb_dimensions = points.GetNumberOfDimensions();

  // save output points
  std::string points_filename = args_info.output_arg;
  points_filename.append(".pts");
  points.Save(points_filename);

  // save output image
  std::string img_filename = args_info.output_arg;
  img_filename.append(".mhd");
  Image4DType::Pointer input_vector_image = builder.GetInputVectorImage();
  syd::WriteImage<Image4DType>(input_vector_image, img_filename);


  double min = std::numeric_limits<double>::max();
  double max = std::numeric_limits<double>::lowest();
  int col=0;
  for(auto x:points) {
    if (x[col]>max) max = x[col];
    if (x[col]<min) min = x[col];
  }

  syd::Histogram h;
  h.SetMinMaxBins(min, max, 20);
  for(auto x:points) h.Fill(x[col]);
  DD(h);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
