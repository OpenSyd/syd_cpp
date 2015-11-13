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
#include "sydSubstituteRadionuclideImageBuilder.h"
#include "sydRoiStatisticBuilder.h"
#include "sydImageFillHoles.h"
#include "sydImageBuilder.h"
#include "sydImage_GaussianFilter.h"

// itk in syd source
#include <itkMedianWithMaskImageFilter.h>

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init ceres log
  SYD_CERES_STATIC_INIT;

  // Init
  SYD_INIT_GGO(sydInsertIntegratedActivityImage, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

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

  // Sort images
  db->Sort<syd::Image>(images);

  // Check same pixel units (warning)
  syd::PixelValueUnit::pointer unit = images[0]->pixel_value_unit;
  for(auto image:images) {
    if (image->pixel_value_unit->id != unit->id) {
      LOG(WARNING) << "I expected pixel value unit to be the same for all images, while it is "
                   << image->pixel_value_unit->name << "for the image:"
                   << std::endl << image
                   << std::endl << " and " << unit->name << " for the image"
                   << std::endl << images[0];
    }
  }

  // image type
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef itk::ImageRegionIterator<ImageType> Iterator;
  typedef itk::NeighborhoodIterator<ImageType> NIterator;

  // Read and keep initial image (for mask)
  ImageType::Pointer initial_image = syd::ReadImage<ImageType>(db->GetAbsolutePath(images[0]));

  // Apply radionuclide substitution. The temporary images are not
  // inserted into the db (but files are created on disk and deleted
  // at the end).
  if (args_info.substitute_given) {
    std::string radname = args_info.substitute_arg;
    syd::Radionuclide::pointer rad;
    odb::query<syd::Radionuclide> q = odb::query<syd::Radionuclide>::name == radname;
    db->QueryOne(rad, q); // will fail if not found
    syd::SubstituteRadionuclideImageBuilder builder(db);
    LOG(2) << "Substitute spect data with radionuclide: " << rad;
    for(auto & image:images) {
      image = builder.NewRadionuclideSubstitutedImage(image, rad);
    }
  }

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
    if (!is) {
      LOG(FATAL) << "Cannot open " << file;
    }
    while (is) {
      debug_point d;
      is >> d.name >> d.x >> d.y >> d.z;
      if (is and d.name[0] != '#') debug_points.push_back(d);
    }
  }

  // Create some models
  std::vector<syd::FitModelBase*> models;
  auto f2  = new syd::FitModel_f2;
  auto f3  = new syd::FitModel_f3;
  auto f4a = new syd::FitModel_f4a;
  auto f4  = new syd::FitModel_f4;
  models.push_back(f2);
  models.push_back(f3);
  models.push_back(f4a);
  models.push_back(f4);

  // Create some optional output types
  auto r2 = new syd::FitOutputImage_R2(im);
  auto best_model = new syd::FitOutputImage_Model(im);
  auto iter = new syd::FitOutputImage_Iteration(im);
  auto eff_half_life = new syd::FitOutputImage_EffHalfLife(im);
  auto nb_points = new syd::FitOutputImage_NbOfPointsForFit(im);
  auto lambda = new syd::FitOutputImage_Lambda(im);

  // Use a mask, consider values of the first spect
  int nb_pixel = 0.0;
  ImageType::Pointer mask = syd::CreateImageLike<ImageType>(initial_image);
  syd::WriteImage<ImageType>(initial_image, "initia.mhd");
  Iterator it_mask(mask, mask->GetLargestPossibleRegion());
  Iterator it_image(initial_image, initial_image->GetLargestPossibleRegion());
  it_mask.GoToBegin();
  it_image.GoToBegin();
  while (!it_mask.IsAtEnd()) {
    if (it_image.Get() > args_info.min_activity_arg) {
      it_mask.Set(1.0);
      ++nb_pixel;
    }
    else it_mask.Set(0.0);
    ++it_mask;
    ++it_image;
  }
  if (args_info.debug_images_flag) syd::WriteImage<ImageType>(mask, "mask.mhd");
  LOG(2) << "I find " << nb_pixel << " pixels to integrate in the mask.";

  // Create main builder
  syd::IntegratedActivityImageBuilder builder;
  for(auto i=0; i<times.size(); i++) builder.AddInput(itk_images[i], times[i]);

  // Options
  builder.SetLambdaPhysicHours(injection->GetLambdaInHours());
  builder.SetDebugOnlyFlag(args_info.debug_only_flag);
  builder.SetR2MinThreshold(args_info.r2_min_arg);
  builder.SetRestrictedTACFlag(args_info.restricted_tac_flag);

  for(auto d:debug_points) builder.AddDebugPixel(d.name, d.x, d.y, d.z);
  if (args_info.debug_images_flag) {
    builder.AddOutputImage(r2);
    builder.AddOutputImage(best_model);
    builder.AddOutputImage(iter);
    builder.AddOutputImage(eff_half_life);
    builder.AddOutputImage(nb_points);
    nb_points->restricted_tac_flag_ = args_info.restricted_tac_flag;
    builder.AddOutputImage(lambda);
  }
  builder.SetMask(mask);

  for(auto i=0; i<args_info.model_given; i++) {
    std::string n = args_info.model_arg[i];
    bool b = false;
    for(auto m:models) {
      if (m->GetName() == n) {
        builder.AddModel(m, i+1); // start model id at 1 (such that 0 means no model)
        b = true;
      }
    }
    if (!b) {
      std::string km;
      for(auto m:models) km += m->GetName()+" ";
      LOG(FATAL) << "Error the model '" << n << "' is not found. Known models are: " << km;
    }
  }
  if (args_info.model_given == 0) {
    LOG(FATAL) << "At least a model must be given (--model).";
  }

  // Go !
  builder.CreateIntegratedActivityImage();

  // Get main output
  auto auc = builder.GetOutput();
  auto success = builder.GetSuccessOutput();

  // Post processing with median filter
  if (args_info.median_filter_flag) {
    LOG(1) << "Post processing: median filter";
    syd::WriteImage<ImageType>(auc->image, "auc_before_median.mhd");
    auto filter = itk::MedianWithMaskImageFilter<ImageType, ImageType, ImageType>::New();
    filter->SetRadius(1);
    filter->SetInput(auc->image);
    filter->SetMask(success->image);
    filter->Update();
    auc->image = filter->GetOutput();
  }

  // Post processing with fill holes
  if (args_info.fill_holes_given) {
    syd::WriteImage<ImageType>(auc->image, "auc_before_fill_holes.mhd");
    // Change the mask, considering success fit
    auto it_success = success->iterator;
    Iterator it_mask(mask, mask->GetLargestPossibleRegion());
    it_success.GoToBegin();
    it_mask.GoToBegin();
    while (!it_mask.IsAtEnd()) {
      if (it_success.Get() == 1.0) it_mask.Set(0.0);
      ++it_success;
      ++it_mask;
    }
    int f = syd::FillHoles<ImageType>(auc->image, mask, args_info.fill_holes_arg);
    LOG(1) << "Post processing: fill remaining holes. " << f << " failed pixels remain.";
  }

  // Write all debug outputs
  if (args_info.debug_images_flag and !args_info.debug_only_flag)
    for(auto o:builder.GetOutputs()) syd::WriteImage<ImageType>(o->image, o->filename);

  // Debug here
  builder.SaveDebugPixel("tac.txt");
  builder.SaveDebugModel("models.txt");

  // Insert result in db
  syd::ImageBuilder bdb(db);
  syd::Image::pointer output = bdb.NewMHDImageLike(images[0]);
  bdb.SetImage<PixelType>(output, auc->image);

  // Tags
  db->SetImageTagsFromCommandLine(output, args_info);

  // Change pixel value
  syd::PixelValueUnit::pointer v = db->FindOrInsertUnit("Bq.h_by_IA", "Time integrated Bq (Bq.h) by injected activity in MBq");
  output->pixel_value_unit = v;

  bdb.InsertAndRename(output);
  LOG(1) << "Inserting Image " << output;

  // Remove temporary files (not persistant in the db)
  if (args_info.substitute_given) {
      for(auto & image:images) {
      for(auto f:image->files) fs::remove_all(db->GetAbsolutePath(f));
    }
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
