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
#include "sydTimeIntegratedActivityImageBuilder.h"
#include "sydImage_GaussianFilter.h"
#include "sydImageFillHoles.h"

// itk
#include <itkMedianWithMaskImageFilter.h>
#include <itkXorImageFilter.h>

// --------------------------------------------------------------------
syd::TimeIntegratedActivityImageBuilder::TimeIntegratedActivityImageBuilder(syd::StandardDatabase * db):
  syd::ImageBuilder(db),
  syd::TimeIntegratedActivityFilter()
{
  UseTimesFromImage(false);
  min_activity_ = 0.0;
  debug_images_flag_ = false;
  median_flag_ = false;
  fill_holes_radius_ = -1;
  gauss_ = -1;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::SetInput(const syd::Image::vector images)
{
  inputs_ = images;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::SetPreProcessingGaussianFilter(double g)
{
  gauss_ = g;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::SetMinimumValueMask(const double min_activity)
{
  min_activity_ = min_activity;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::SetDebugImagesFlag(bool debug)
{
  debug_images_flag_ = debug;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::SetPostProcessingMedianFilter(bool median)
{
  median_flag_ = median;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::SetPostProcessingFillHoles(int radius)
{
  fill_holes_radius_ = radius;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TimeIntegratedActivityImageBuilder::PrintOptions() const
{
  std::stringstream s;
  s << "Input images=";
  for(auto im:inputs_) s << im->id << " ";
  s << "min_r2=" << R2_min_threshold_
    << " restricted=" << restricted_tac_flag_
    << " models=";
  for(auto m:models_) s << m->GetName() << " ";
  s
    << " pre_mask=" << min_activity_
    << " pre_gauss=" << gauss_
    << " post_median=" << median_flag_
    << " post_fill_holes=" << fill_holes_radius_;
  return s.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::CreateTimeIntegratedActivityImage()
{
  // Sort images
  db_->Sort<syd::Image>(inputs_);

  // Check images, injection
  syd::Injection::pointer injection = inputs_[0]->injection;
  bool b = true;
  for(auto image:inputs_) {
    // image->FatalIfNoDicom();
    b = b and (injection->id == image->injection->id);
  }
  if (!b) {
    EXCEPTION("The image do not have the same injection.");
  }
  SetLambdaPhysicHours(injection->GetLambdaInHours());

  // Get inputs times from image or from user
  std::vector<double> temp_times;
  if (use_time_from_image_flag_) {
    std::string starting_date = injection->date;
    for(auto image:inputs_) {
      double t = syd::DateDifferenceInHours(image->dicoms[0]->acquisition_date, starting_date);
      temp_times.push_back(t);
    }
  }
  else temp_times = times_;
  if (temp_times.size() != inputs_.size()) {
    LOG(FATAL) << "Error nb of 'times' must be the same than nb of images, while : "
               << times_.size() << " " << inputs_.size();
  }

  // Load initial itk_images
  std::vector<ImageType::Pointer> initial_images;
  for(auto image:inputs_) {
    auto im = syd::ReadImage<ImageType>(db_->GetAbsolutePath(image));
    initial_images.push_back(im);
  }

  // Create mask if needed
  RunPreProcessing(initial_images);

  // Set inputs
  times_.clear(); // important
  images_.clear();
  for(auto i=0; i<temp_times.size(); i++)
    AddInput(initial_images[i], temp_times[i]);

  // debug image if needed
  if (debug_images_flag_) {
    auto r2 = new syd::FitOutputImage_R2();
    auto best_model = new syd::FitOutputImage_Model();
    auto iter = new syd::FitOutputImage_Iteration();
    //auto eff_half_life = new syd::FitOutputImage_EffHalfLife();
    //auto nb_points = new syd::FitOutputImage_NbOfPointsForFit();
    //auto lambda = new syd::FitOutputImage_Lambda();
    auto image_params = new syd::FitOutputImage_ModelParams();
    AddOutputImage(r2);
    AddOutputImage(best_model);
    AddOutputImage(iter);
    //AddOutputImage(eff_half_life);
    //AddOutputImage(nb_points);
    //AddOutputImage(lambda);
    AddOutputImage(image_params);
  }

  // Go !
  std::string sm;
  for(auto m:models_) sm += m->name_+" ("+syd::ToString(m->id_)+") ";
  LOG(2) << "Starting fit: models= " << sm << "; "
         << (min_activity_>0.0 ? "with mask":"no_mask")
         << " pixels=" << nb_pixels_
         << " R2_min= " << R2_min_threshold_
         << (restricted_tac_flag_ ? " restricted":" non restricted");
  CreateIntegratedActivityImage();

  // create the output image
  tia_ = NewMHDImageLike(inputs_[0]);
  auto fit_output = GetOutput();
  SetImage<PixelType>(tia_, fit_output->image);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::RunPreProcessing(std::vector<ImageType::Pointer> & images)
{
  // Mask
  if (min_activity_> 0.0) {
    // Create image with max value along the sequence
    ImageType::Pointer first_image = images[0];
    ImageType::Pointer mask = syd::CreateImageLike<ImageType>(first_image);
    std::vector<Iterator3D> it;
    for(auto image:images) {
      Iterator3D i = Iterator3D(image, image->GetLargestPossibleRegion());
      i.GoToBegin();
      it.push_back(i);
    }
    Iterator3D it_mask(mask, mask->GetLargestPossibleRegion());
    nb_pixels_ = 0.0;
    while (!it_mask.IsAtEnd()) {
      PixelType maxi = it[0].Get();
      for(auto & itt:it) maxi = std::max(maxi, itt.Get());
      if (maxi > min_activity_) {
        it_mask.Set(1);
        ++nb_pixels_;
      }
      else it_mask.Set(0);
      ++it_mask;
      for(auto & itt:it) ++itt;
    }
    SetMask(mask);
  }
  else {
    nb_pixels_ = images[0]->GetLargestPossibleRegion().GetNumberOfPixels();
  }

  // Gauss
  if (gauss_ > 0.0)
    for(auto & image:images)
      image = syd::GaussianFilter<ImageType>(image, gauss_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::RunPostProcessing()
{
  if (median_flag_ or fill_holes_radius_>0) {
    auto output = GetOutput()->image;
    auto success = success_output_->image;
    auto initial_mask = mask_;

    typedef itk::CastImageFilter<ImageType, MaskImageType> CastFilterType;
    CastFilterType::Pointer cast1 = CastFilterType::New();
    cast1->SetInput(initial_mask);
    CastFilterType::Pointer cast2 = CastFilterType::New();
    cast2->SetInput(success);
    typedef itk::XorImageFilter<MaskImageType, MaskImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput1(cast1->GetOutput());
    filter->SetInput2(cast2->GetOutput());
    filter->Update();
    auto working_mask = filter->GetOutput();
    typedef itk::CastImageFilter<MaskImageType, ImageType> CastFilterType2;
    CastFilterType2::Pointer cast3 = CastFilterType2::New();
    cast3->SetInput(working_mask);
    cast3->Update();

    // Median
    if (median_flag_) {
      LOG(FATAL) << "Not implemented yet";
      LOG(1) << "Post processing: median filter";
      auto filter = itk::MedianWithMaskImageFilter<ImageType, ImageType, ImageType>::New();
      filter->SetRadius(1);
      filter->SetInput(output);
      filter->SetMask(cast3->GetOutput());
      filter->Update();
      output = filter->GetOutput();
    }

    // Fill_Holes
    if (fill_holes_radius_ > 0) {
      int f = syd::FillHoles<ImageType>(output,  cast3->GetOutput(), fill_holes_radius_);
      LOG(1) << "Post processing: fill remaining holes. " << f << " failed pixels remain.";
      // syd::WriteImage<ImageType>(output, "fill_holes.mhd");
    }

    //GetOutput()->image = output;
    SetImage<PixelType>(tia_, output); // set the real output
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::InsertOutputImagesInDB(std::vector<std::string> & tag_names)
{
  for(auto o:outputs_) {
    // ignore some debug
    if (o->filename == "integrate.mhd") continue;
    if (o->filename == "auc.mhd") continue;

    // create the output image
    auto output_image = NewMHDImageLike(inputs_[0]);
    // special case for 4D image params
    if (o->filename == "params.mhd") {
      syd::FitOutputImage_ModelParams * oo =
        static_cast<syd::FitOutputImage_ModelParams*>(o);
      typedef syd::FitOutputImage_ModelParams::Image4DType Image4DType;
      syd::WriteImage<Image4DType>(oo->image_4d, db_->GetAbsolutePath(output_image));
      std::string md5 = syd::ComputeImageMD5<Image4DType>(oo->image_4d);
      output_image->files[1]->md5 = md5;
    }
    else {
      SetImage<PixelType>(output_image, o->image);
    }
    // Tags
    syd::Tag::pointer tag;
    db_->FindTag(tag, o->tag);
    AddTag(output_image->tags, tag);

    syd::Tag::vector tags;
    db_->FindTags(tags, tag_names);
    AddTag(output_image->tags, tags);

    // PixelUnits
    // TODO

    // update
    InsertAndRename(output_image);
  }

  if (mask_) {
    auto mask_image = NewMHDImageLike(inputs_[0]);
    SetImage<PixelType>(mask_image, mask_);
    // Tags
    syd::Tag::pointer tag;
    db_->FindTag(tag, "fit_initial_mask");
    AddTag(mask_image->tags, tag);
    syd::Tag::vector tags;
    db_->FindTags(tags, tag_names);
    AddTag(mask_image->tags, tags);
    InsertAndRename(mask_image);
  }

}
// --------------------------------------------------------------------
