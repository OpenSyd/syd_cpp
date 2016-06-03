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

// syd init
SYD_STATIC_INIT

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
  DD(mask);

  // Input tia image
  id = atoi(args_info.inputs[1]);
  syd::Image::pointer tia;
  db->QueryOne(tia, id);
  DD(tia);

  // Input model
  id = atoi(args_info.inputs[2]);
  syd::Image::pointer model;
  db->QueryOne(model, id);
  DD(model);

  // Input params (4D image)
  id = atoi(args_info.inputs[3]);
  syd::Image::pointer params;
  db->QueryOne(params, id);
  DD(params);

  // Main builder
  syd::KmeansInputDataBuilder builder; //no db ?)
  typedef syd::KmeansInputDataBuilder::ImageType ImageType;
  typedef syd::KmeansInputDataBuilder::Image4DType Image4DType;
  typedef syd::KmeansInputDataBuilder::InputKmeanType InputKmeanType;
  builder.SetMask(syd::ReadImage<ImageType>(db->GetAbsolutePath(mask)));
  builder.AddInput(syd::ReadImage<ImageType>(db->GetAbsolutePath(tia)));
  builder.AddInput(syd::ReadImage<ImageType>(db->GetAbsolutePath(model)));
  // consider the index for the params images
  std::vector<int> indices = {0,1,2};
  builder.AddInput(syd::ReadImage<Image4DType>(db->GetAbsolutePath(params)), indices);
  // FIXME gauss
  // FIXME normalize

  // Go
  DD("go");
  builder.BuildInputData();
  DD("end");

  // Get output
  InputKmeanType & v = builder.GetInputKmeansData();
  int nb_dimensions = builder.GetNumberOfDimensions();
  DD(nb_dimensions);

  Image4DType::Pointer input_vector_image = builder.GetInputVectorImage();

  syd::WriteImage<Image4DType>(input_vector_image, "a.mhd");
  //FIXME write v;

  DD("done");

  // This is the end, my friend.
}
// --------------------------------------------------------------------
