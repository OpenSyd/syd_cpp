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
#include "sydImageFillHoles.h"
#include "sydImage_GaussianFilter.h"

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
  if (images.size() ==0) {
    LOG(1) << "No images.";
    return EXIT_SUCCESS;
  }

  // Check same injection date
  images[0]->FatalIfNoDicom();
  syd::Injection::pointer injection = images[0]->dicoms[0]->injection;
  bool b = true;
  for(auto image:images) {
    image->FatalIfNoDicom();
    b = b and (injection->id == image->dicoms[0]->injection->id);
  }
  if (!b) {
    LOG(FATAL) << "The image do not have the same injection.";
  }

  // Check same pixel units (warning)
  for(auto image:images) {
    if (image->pixel_value_unit->name != "MBq_by_IA") {
      LOG(WARNING) << "I expected pixel value unit to be 'MBq_by_IA', while it is "
                   << image->pixel_value_unit->name << "for the image:"
                   << std::endl << image;
    }
  }

  // image type
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef itk::ImageRegionIterator<ImageType> Iterator;
  typedef itk::NeighborhoodIterator<ImageType> NIterator;

  // Read the images+times and set to the builder
  std::string starting_date = injection->date;
  ImageType::Pointer im;
  std::vector<ImageType::Pointer> itk_images;
  std::vector<double> times;
  for(auto image:images) {
    im = syd::ReadImage<ImageType>(db->GetAbsolutePath(image));
    if (args_info.gauss_arg != 0) im = syd::GaussianFilter<ImageType>(im, args_info.gauss_arg);
    double t = syd::DateDifferenceInHours(image->dicoms[0]->acquisition_date, starting_date);
    times.push_back(t);
    itk_images.push_back(im);
  }
  im = itk_images[0]; // consider the first image for the following

  // Consider the debug points
  struct debug_point {
    std::string name;
    int x,y,z;
  };
  std::vector<debug_point> debug_points;
  if (args_info.debug_given) {
    std::string file=args_info.debug_arg;
    std::ifstream is(file);
    while (is) {
      debug_point d;
      is >> d.name >> d.x >> d.y >> d.z;
      if (is and d.name[0] != '#') debug_points.push_back(d);
    }
  }

  // Create some models
  auto f2  = new syd::FitModel_f2;
  auto f3  = new syd::FitModel_f3;
  auto f4a = new syd::FitModel_f4a;
  auto f4  = new syd::FitModel_f4;

  // Create some output types
  auto auc = new syd::FitOutputImage_AUC(im, injection->GetLambdaInHours());
  auto r2 = new syd::FitOutputImage_R2(im);
  auto best_model = new syd::FitOutputImage_Model(im);
  auto iter = new syd::FitOutputImage_Iteration(im);
  auto success = new syd::FitOutputImage_Success(im);
  auto eff_half_life = new syd::FitOutputImage_EffHalfLife(im);
  auto nb_points = new syd::FitOutputImage_NbOfPointsForFit(im);

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
  if (args_info.debug_images_flag)
    syd::WriteImage<ImageType>(mask, "mask.mhd");


  // Create main builder
  syd::IntegratedActivityImageBuilder builder;
  for(auto i=0; i<times.size(); i++) builder.AddInput(itk_images[i], times[i]);

  // FIXME
  builder.image_lambda_phys_in_hour_ = injection->GetLambdaInHours();
  builder.debug_only_flag_ = args_info.debug_only_flag;
  builder.R2_min_threshold_ = args_info.r2_min_arg;
  builder.restricted_tac_flag_ = true; // FIXME

  for(auto d:debug_points) builder.AddDebugPixel(d.name, d.x, d.y, d.z);
  builder.AddOutputImage(auc);
  if (args_info.debug_images_flag) {
    builder.AddOutputImage(success);
    builder.AddOutputImage(r2);
    builder.AddOutputImage(best_model);
    builder.AddOutputImage(iter);
    builder.AddOutputImage(eff_half_life);
    builder.AddOutputImage(nb_points);
  }
  builder.SetMask(mask);

  builder.AddModel(f2, 2); // no f2 ?
  builder.AddModel(f3, 3);
  builder.AddModel(f4a, 4);
  builder.AddModel(f4, 5); // no f4 ?

  // Go !
  builder.CreateIntegratedActivityImage();

  // Output FIXME
  if (args_info.debug_images_flag and !args_info.debug_only_flag)
    for(auto o:builder.outputs_) syd::WriteImage<ImageType>(o->image, o->filename);

  // Debug here
  builder.SaveDebugPixel("gp/tac.txt");
  builder.SaveDebugModel("gp/models.txt");

  // // Modifiy the mask according to success
  // auto it_success = success->iterator;
  // it_success.GoToBegin();
  // it_mask.GoToBegin();
  // while (!it_mask.IsAtEnd()) {
  //   if (it_success.Get() == 1.0) it_mask.Set(0.0);
  //   ++it_success;
  //   ++it_mask;
  // }
  // if (args_info.debug_images_flag and !args_info.debug_only_flag)
  //   syd::WriteImage<ImageType>(mask, "mask2.mhd");

  // // Redo with a mask
  // builder.ClearModel();
  // builder.debug_data.clear(); // need to reset the debug data
  // for(auto d:debug_points) builder.AddDebugPixel(d.name, d.x, d.y, d.z);
  // f3->id_ = 5; // to distinguish from previous
  // builder.AddModel(f3);
  // f4a->id_ = 6; // to distinguish from previous
  // builder.AddModel(f4a);
  // builder.restricted_tac_flag_ = true;
  // builder.CreateIntegratedActivityImage();

  // // Output
  // if (args_info.debug_images_flag and !args_info.debug_only_flag)
  //   for(auto o:builder.outputs_) syd::WriteImage<ImageType>(o->image, o->filename+"_2.mhd");

  // // Debug here
  // builder.SaveDebugPixel("gp/tac_2.txt");
  // builder.SaveDebugModel("gp/models_2.txt");

  // // update the mask
  // if (!args_info.debug_only_flag) {
  //   it_success = success->iterator;
  //   it_success.GoToBegin();
  //   it_mask.GoToBegin();
  //   while (!it_mask.IsAtEnd()) {
  //     if (it_success.Get() == 1.0) it_mask.Set(0.0);
  //     ++it_success;
  //     ++it_mask;
  //   }
  //   if (args_info.debug_images_flag and !args_info.debug_only_flag)
  //     syd::WriteImage<ImageType>(mask, "mask3.mhd");

  //   int f = syd::FillHoles<ImageType>(auc->image, mask, 2); FIXME

  //   // Deal with remaining failed pixels
  //   LOG(1) << "Last step: fill remaining holes. " << f << " failed pixels remain.";

  //   syd::WriteImage<ImageType>(auc->image, "auc3.mhd");
  // }

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
