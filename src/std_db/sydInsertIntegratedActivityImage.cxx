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
#include "sydInsertIntegratedActivityImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"
#include "sydIntegratedActivityImageBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init ceres log
  SYD_CERES_STATIC_INIT;

  // Init
  SYD_INIT_GGO(sydInsertIntegratedActivityImage, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the tag
  std::string tagname = args_info.inputs[0];
  syd::Tag::vector tags;
  db->FindTags(tags, tagname);

  // Get the list of images to integrate
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=1; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::Image::vector images;
  db->Query(images, ids);

  // Sort by time
  std::sort(begin(images), end(images),
            [images](syd::Image::pointer a, syd::Image::pointer b) {
              if (a->dicoms.size() == 0) return true;
              if (b->dicoms.size() == 0) return false;
              return a->dicoms[0]->acquisition_date < b->dicoms[0]->acquisition_date;
            });

  // FIXME
  DD("FIXME : make some check on images list here");

  // image type
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef itk::ImageRegionIterator<ImageType> Iterator;

  // Create main builder
  syd::IntegratedActivityImageBuilder builder;

  // Read the images+times and set to the builder
  std::string starting_date = images[0]->dicoms[0]->injection->date;
  ImageType::Pointer im;
  std::vector<ImageType::Pointer> itk_images;
  for(auto image:images) {
    im = syd::ReadImage<ImageType>(db->GetAbsolutePath(image));
    if (args_info.gauss_arg != 0) im = syd::GaussianFilter<ImageType>(im, args_info.gauss_arg);
    double t = syd::DateDifferenceInHours(image->dicoms[0]->acquisition_date, starting_date);
    builder.AddInput(im, t);
    itk_images.push_back(im);
  }
  im = itk_images[0]; // consider the first image for the following

  // Set some options
  builder.image_lambda_phys_in_hour_ = log(2.0)/images[0]->dicoms[0]->injection->radionuclide->half_life_in_hours;
  builder.debug_only_flag_ = args_info.only_debug_flag;
  builder.robust_scaling_ = args_info.robust_scaling_arg;
  builder.R2_min_threshold_ = args_info.r2_min_arg;

  if (args_info.debug_given) {
    std::string file=args_info.debug_arg;
    std::ifstream is(file);
    while (is) {
      std::string name;
      int x,y,z;
      is >> name >> x >> y >> z;
      if (is) {
        if (name[0] != '#') builder.AddDebugPixel(name, x,y,z);
      }
    }
  }

  // Set the models
  auto f3 = new syd::FitModel_f3;
  auto f4a = new syd::FitModel_f4a;
  builder.AddModel(f3);
  builder.AddModel(f4a);

  // Set the output
  auto auc = new syd::FitOutputImage_AUC(im);
  auto r2 = new syd::FitOutputImage_R2(im);
  auto best_model = new syd::FitOutputImage_Model(im);
  auto iter = new syd::FitOutputImage_Iteration(im);
  auto success = new syd::FitOutputImage_Success(im);
  builder.AddOutputImage(auc);
  builder.AddOutputImage(r2);
  builder.AddOutputImage(best_model);
  builder.AddOutputImage(iter);
  builder.AddOutputImage(success);

  // Use a mask, consider values of the first spect
  ImageType::Pointer mask = syd::CreateImageLike<ImageType>(im);
  Iterator it_mask(mask, mask->GetLargestPossibleRegion());
  Iterator it_image(im, im->GetLargestPossibleRegion());
  it_mask.GoToBegin();
  it_image.GoToBegin();
  while (!it_mask.IsAtEnd()) {
    if (it_image.Get() > args_info.min_activity_arg) it_mask.Set(1.0);
    else it_mask.Set(0.0);
    ++it_mask;
    ++it_image;
  }
  builder.SetMask(mask);
  syd::WriteImage<ImageType>(mask, "mask.mhd");

  // Go !
  builder.CreateIntegratedActivityImage();

  // Output
  for(auto o:builder.outputs_) syd::WriteImage<ImageType>(o->image, o->filename);

  // Debug here //FIXME
  builder.SaveDebugPixel("gp/tac.txt");
  builder.SaveDebugModel("gp/models.txt");

  // Modifiy the mask according to success
  auto it_success = success->iterator;
  it_success.GoToBegin();
  it_mask.GoToBegin();
  while (!it_mask.IsAtEnd()) {
    if (it_success.Get() == 1.0) it_mask.Set(0.0);
    ++it_success;
    ++it_mask;
  }
  syd::WriteImage<ImageType>(mask, "mask2.mhd");

  // Redo with a mask
  DD("Start again");
  builder.ClearModel();
  f3->id_ = f3->id_*10; // to distinguish from previous
  f3->start_from_max_flag = true;
  builder.AddModel(f3);
  builder.restricted_tac_flag_ = true;
  builder.CreateIntegratedActivityImage();

  // Output
  //for(auto o:builder.outputs_) syd::WriteImage<ImageType>(o->image, o->filename);
  syd::WriteImage<ImageType>(r2->image, "r2_bis.mhd");
  syd::WriteImage<ImageType>(best_model->image, "best_model_bis.mhd");
  syd::WriteImage<ImageType>(auc->image, "auc_bis.mhd");

  // Debug here //FIXME
  builder.SaveDebugPixel("gp/tac_2.txt");
  builder.SaveDebugModel("gp/models_2.txt");

  // Output
  DD("FIXME : insert builder output in the db");

  // Update tags
  /*
  for(auto t:tags) image->AddTag(t);
  db->Update(image);
  LOG(1) << "Inserting Image " << image;
  */

  DD("done");
  // This is the end, my friend.
}
// --------------------------------------------------------------------
