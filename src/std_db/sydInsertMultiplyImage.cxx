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
#include "sydInsertMultiplyImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydRoiStatisticBuilder.h"
#include "sydCommonGengetopt.h"
#include "sydScaleImageBuilder.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertMultiplyImage, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the image1 id
  syd::IdType id1 = atoi(args_info.inputs[0]);
  syd::Image::pointer image1;
  db->QueryOne(image1, id1);

  // Get the other images
  std::vector<syd::IdType> ids;
  for(auto i=1; i<args_info.inputs_num; i++)
    ids.push_back(atoi(args_info.inputs[i]));
  syd::Image::vector images;
  db->Query(images, ids);

  // Read first itk image
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  auto itk_image1 = syd::ReadImage<ImageType>(db->GetAbsolutePath(image1));

  // Create the images
  syd::ImageBuilder builder(db);
  for(auto image2:images) {
    auto itk_image2 = syd::ReadImage<ImageType>(db->GetAbsolutePath(image2));

    // Resample
    LOG(2) << "Resample image2 like image1";
    itk_image2 = syd::ResampleAndCropImageLike<ImageType>(itk_image2, itk_image1, 1, 0.0);

    // Multiply
    if (!args_info.inverse_flag) {
      LOG(2) << "Multiply images " << id1 << " by image " << image2->id;
      typedef itk::MultiplyImageFilter<ImageType, ImageType, ImageType> FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->SetInput1(itk_image1);
      filter->SetInput2(itk_image2);
      filter->Update();
      itk_image2 = filter->GetOutput();
    }
    else {
      LOG(2) << "Divide images " << id1 << " by image " << image2->id;
      typedef itk::DivideImageFilter<ImageType, ImageType, ImageType> FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->SetInput1(itk_image1);
      filter->SetInput2(itk_image2);
      filter->Update();
      itk_image2 = filter->GetOutput();
    }

    // Insert the new image
    LOG(2) << "Store in the db";
    auto output = builder.NewMHDImageLike(image2);
    builder.SetImage<PixelType>(output, itk_image2);
    builder.InsertAndRename(output);
    db->UpdateTagsFromCommandLine(output->tags, args_info);
    if (args_info.pixelunit_given) builder.SetImagePixelValueUnit(output, args_info.pixelunit_arg);
    db->Update(output);
    LOG(1) << "Image inserted: " << output;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
