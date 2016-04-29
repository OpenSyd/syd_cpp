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

// --------------------------------------------------------------------
syd::TimeIntegratedActivityImageBuilder::TimeIntegratedActivityImageBuilder(syd::StandardDatabase * db):
  syd::ImageBuilder(db),
  syd::TimeIntegratedActivityFilter()
{
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
    image->FatalIfNoDicom();
    b = b and (injection->id == image->injection->id);
  }
  if (!b) {
    EXCEPTION("The image do not have the same injection.");
  }

  // Get inputs times
  std::string starting_date = injection->date;
  std::vector<double> times;
  for(auto image:inputs_) {
    double t = syd::DateDifferenceInHours(image->dicoms[0]->acquisition_date, starting_date);
    times.push_back(t);
  }

  // Load initial itk_images
  std::vector<ImageType::Pointer> initial_images;
  for(auto image:inputs_) {
    auto im = syd::ReadImage<ImageType>(db_->GetAbsolutePath(image));
    initial_images.push_back(im);
  }

  // Create mask if needed
  RunPreProcessing(initial_images);

  // FIXME additional point

  // Set inputs
  for(auto i=0; i<times.size(); i++)
    AddInput(initial_images[i], times[i]);

  // debug image if needed
  if (debug_images_flag_) {
    auto r2 = new syd::FitOutputImage_R2();
    auto best_model = new syd::FitOutputImage_Model();
    auto iter = new syd::FitOutputImage_Iteration();
    auto eff_half_life = new syd::FitOutputImage_EffHalfLife();
    auto nb_points = new syd::FitOutputImage_NbOfPointsForFit();
    auto lambda = new syd::FitOutputImage_Lambda();
    AddOutputImage(r2);
    AddOutputImage(best_model);
    AddOutputImage(iter);
    AddOutputImage(eff_half_life);
    AddOutputImage(nb_points);
    AddOutputImage(lambda);
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
  DD("todo post process");

}
// --------------------------------------------------------------------
