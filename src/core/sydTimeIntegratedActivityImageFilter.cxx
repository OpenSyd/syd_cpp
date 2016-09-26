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
#include "sydTimeIntegratedActivityImageFilter.h"
#include "sydImageUtils.h"

// itk
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>

// --------------------------------------------------------------------
syd::TimeIntegratedActivityImageFilter::TimeIntegratedActivityImageFilter()
{
  DD("TimeIntegratedActivityImageFilter");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeIntegratedActivityImageFilter::~TimeIntegratedActivityImageFilter()
{
  DD("~TimeIntegratedActivityImageFilter");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::Run()
{
  DD("TimeIntegratedActivityImageFilter::Run");

  // Check inputs: size, times, negative values ?
  CheckInputs();

  // Initialisation: mask (create if do not exist)
  InitMask();

  // Initialisation: create 4D images
  Init4DInput();

  // Initialisation: FitOutputImage
  InitOutputs();

  // Initialisation: Models
  InitModels();

  // Initialisation: Solver
  InitSolver();

  // Initialisation: iterators (both input and output)
  Iterator4D it(tac_image_, tac_image_->GetLargestPossibleRegion());
  IteratorMask it_mask(mask_, mask_->GetLargestPossibleRegion());
  it_mask.GoToBegin();
  for(auto & o:outputs_) o->iterator.GoToBegin();

  // Main loop
  int x = 0;
  int n = images_[0]->GetLargestPossibleRegion().GetNumberOfPixels();
  DD(n);
  for (it.GoToBegin(); !it.IsAtEnd(); ) {

    if (it_mask.Get() != 0) { // inside the mask
      FitOnePixel(it);
    }
    else {
      // FIXME update output
    }

    // progress bar
    ++x;
    if (sydlog::Log::LogLevel() > 0) loadbar(x,n);

    // Next pixel
    for(auto o:outputs_) o->Iterate();
    ++it_mask;
    ++it;
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::FitOnePixel(Iterator4D & it)
{
  DDF();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::InitSolver()
{
  DDF();
  // Solve
  ceres_options_ = new ceres::Solver::Options;
  ceres_options_->max_num_iterations = options_.GetMaxNumIterations();
  ceres_options_->linear_solver_type = ceres::DENSE_QR; // because few parameters/data
  //  ceres_options_->linear_solver_type = ceres::DENSE_SCHUR;
  ceres_options_->minimizer_progress_to_stdout = false;
  ceres_options_->trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT; // LM is the default

  /*
    DD(ceres_options_->min_line_search_step_size);
    DD(ceres_options_->max_line_search_step_contraction);
    DD(ceres_options_->function_tolerance);
    DD(ceres_options_->parameter_tolerance);
    DD(ceres_options_->num_threads);
  */

  //ceres_options_->function_tolerance = 1e-8;

  //ceres_options_->trust_region_strategy_type = ceres::DOGLEG;// (LM seems faster)
  //ceres_options_->dogleg_type = ceres::SUBSPACE_DOGLEG;
  ceres_options_->logging_type = ceres::SILENT;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::InitModels()
{
  DDF();
  models_.clear();
  models_ = options_.GetModels();
  for(auto m:models_) {
    DD(m->GetName());
    m->SetLambdaDecayConstantInHours(lambda_in_hours_);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::InitOutputs()
{
  DDF();
  for(auto & o:outputs_) {
    o->InitImageLike(images_[0]); // allocate images
    //o->iterator.GoToBegin();
    //o->initial_tac_ = initial_tac;
    //o->working_tac_ = working_tac;;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::Init4DInput()
{
  DDF();

  // Create
  tac_image_ = Image4DType::New();

  // Create a 4D images with the set of 3D images
  typename Image4DType::SizeType size;
  for(auto i=1; i<4; i++) size[i] = images_[0]->GetLargestPossibleRegion().GetSize()[i-1];
  size[0] = images_.size();
  DD(size);
  typename Image4DType::RegionType region;
  region.SetSize(size);
  DD(region);
  tac_image_->SetRegions(region);
  DD(region);
  typename Image4DType::SpacingType spacing;
  for(auto i=1; i<4; i++) spacing[i] = images_[0]->GetSpacing()[i-1];
  spacing[0] = 1.0;
  tac_image_->SetSpacing(spacing);
  DD(spacing);
  typename Image4DType::PointType origin;
  for(auto i=1; i<4; i++) origin[i] = images_[0]->GetOrigin()[i-1];
  origin[0] = 0.0;
  tac_image_->SetOrigin(origin);
  DD(origin);
  tac_image_->Allocate();

  // Copy data to the 4D image
  Iterator4D it(tac_image_, tac_image_->GetLargestPossibleRegion());
  std::vector<Iterator> iterators;
  for(auto image:images_) {
    iterators.push_back(Iterator(image, image->GetLargestPossibleRegion()));
  }
  bool negative_values = false;
  for(auto & iter:iterators) iter.GoToBegin();
  for (it.GoToBegin(); !it.IsAtEnd(); ) {
    for(auto & iter:iterators) {
      if (iter.Get() < 0.0) {
        if (!negative_values) {
          LOG(WARNING) << "Find negative count values in the input images, change to 0.";
          negative_values = true;
        }
        it.Set(0.0);
      }
      else it.Set(iter.Get());
      ++iter;
      ++it;
    }
  }
  syd::WriteImage<Image4DType>(tac_image_, "s4d.mhd");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::InitMask()
{
  DDF();
  DD("TODO");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::CheckInputs()
{
  DDF();
  DD("TODO");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::AddInput(ImageType::Pointer image, double time)
{
  DDF();
  images_.push_back(image);
  times_.push_back(time);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::AddOutputImage(syd::FitOutputImage * o)
{
  DDF();
  outputs_.push_back(o);
}
// --------------------------------------------------------------------
