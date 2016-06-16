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
#include "sydInsertRootMeanSquareImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageFromDicomBuilder.h"
#include "sydCommonGengetopt.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertRootMeanSquareImage, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the images
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=1; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::Image::vector images;
  db->Query(images, ids);
  if (images.size() == 0) {
    LOG(FATAL) << "No image ids given. I do nothing.";
  }

  // Get the mask
  std::string mask_name = args_info.inputs[0];
  auto mask = db->FindRoiMaskImage(images[0], mask_name);

  // Read the mhd
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  ImageType::Pointer itk_image1 = syd::ReadImage<ImageType>(db->GetAbsolutePath(images[0]));
  ImageType::Pointer itk_image2 = syd::ReadImage<ImageType>(db->GetAbsolutePath(images[1]));
  ImageType::Pointer itk_output = syd::CreateImageLike<ImageType>(itk_image1);
  ImageType::Pointer itk_mask = syd::ReadImage<ImageType>(db->GetAbsolutePath(mask));

  // Resize mask
  itk_mask = syd::ResampleAndCropImageLike<ImageType>(itk_mask, itk_image1, 0, 0);
  syd::WriteImage<ImageType>(itk_mask, "mask.mhd");


  typedef itk::ImageRegionIterator<ImageType> Iterator;
  Iterator iter1(itk_image1, itk_image1->GetLargestPossibleRegion());
  Iterator iter2(itk_image2, itk_image2->GetLargestPossibleRegion());
  Iterator itero(itk_output, itk_output->GetLargestPossibleRegion());
  Iterator iterm(itk_mask, itk_mask->GetLargestPossibleRegion());
  double mean_auc = 0.0;
  double rmse = 0.0;
  double msd = 0.0;
  int nb = 0;
  while (!iter1.IsAtEnd()) {
    if (iterm.Get() != 0 and iter1.Get() != 0 and iter2.Get() != 0) {
      double e = pow(iter1.Get() - iter2.Get(),2);
      itero.Set(e);
      mean_auc += iter1.Get();
      msd += iter1.Get() - iter2.Get();
      rmse += e;
      nb++;
    }
    else itero.Set(0);
    ++iter1;
    ++iter2;
    ++itero;
    ++iterm;
  }

  mean_auc = mean_auc/nb;
  rmse = sqrt(rmse/nb)/mean_auc*100.0;
  msd = (msd/nb)/mean_auc*100.0;

  itero.GoToBegin();
  iterm.GoToBegin();
  while (!itero.IsAtEnd()) {
    if (iterm.Get() != 0) {
      double r = sqrt(itero.Get())/mean_auc*100.0;
      itero.Set(r);
    }
    ++itero;
    ++iterm;
  }
  syd::WriteImage<ImageType>(itk_output, "rmse.mhd");

  std::cout << rmse << " " << msd << std::endl;

  // Create the new image
  /*
  syd::ImageBuilder builder(db);
  syd::Image::pointer output = builder.NewMHDImageLike(images[0]);


  // Add optional tag
  db->UpdateTagsFromCommandLine(output->tags, args_info);

  // Set optional unity
  syd::PixelValueUnit::pointer unit = db->FindPixelValueUnit("%");
  output->pixel_value_unit = unit;

  // Insert in the db
  builder.InsertAndRename(output);
  LOG(1) << "Inserting Image " << output;
  */
  // This is the end, my friend.
}
// --------------------------------------------------------------------
