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

  // Initialisation TAC
  initial_tac_ = syd::TimeActivityCurve::New();
  for(auto t:times_) initial_tac_->AddValue(t, 0);
  working_tac_ = syd::TimeActivityCurve::New();
  DD("ADD value here");//FIXME

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
      // FIXME update output ?
    }

    // progress bar
    ++x;
    if (sydlog::Log::LogLevel() > 0) loadbar(x,n);

    // Next pixel
    for(auto o:outputs_) o->Iterate();
    ++it_mask;
    // move iterator for all images
    for(auto i=0; i<images_.size(); i++) ++it;
    //    ++it;
  }
  DD("DONE");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::FitOnePixel(Iterator4D it)
{
  // Create initial tac
  for(auto i=0; i<images_.size(); i++) {
    initial_tac_->SetValue(i, it.Get());
    ++it; // next value
  }
  DD(initial_tac_);

  // Create working tac (restricted, + add value)
  if (options_.GetRestrictedFlag()) {
    DD("TODO");
  }
  else working_tac_ = initial_tac_;
  // FIXME: todo add value
  DD(working_tac_);

  // Loop on models
  for(auto model:models_) {
    FitTACWithModel(model, working_tac_);
  }

  // Select best one
  auto best_model_num = SelectBestModel(models_, working_tac_);
  bool success = (best_model_num != -1);
  DD(success);

  // Update the output
  if (success) {
    auto best_model = models_[best_model_num];
    DD(best_model->GetName());
    for(auto & o:outputs_)
      o->Update(best_model);
  }

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
    o->SetInitialTimeActivityCurve(initial_tac_);
    o->SetWorkingTimeActivityCurve(working_tac_);
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

  if (images_.size() < 2) {
    EXCEPTION("Provide at least 2 images before");
  }

  // Check image size
  bool b = true;
  for(auto image:images_)
    b = b and syd::CheckImageSameSizeAndSpacing<ImageType::ImageDimension>(images_[0], image);
  if (!b) {
    EXCEPTION("The images must have the same size/spacing, abort.");
  }

  DD("TODO : check order times ");

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
void syd::TimeIntegratedActivityImageFilter::
AddOutputImage(syd::FitOutputImage::pointer o)
{
  DDF();
  outputs_.push_back(o);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::
FitTACWithModel(syd::FitModelBase::pointer model,
                syd::TimeActivityCurve::pointer tac)
{
  DDF();
  ceres::Problem problem;// New problem each time. (I did not manage to change that)
  model->ComputeStartingParametersValues(tac);
  model->SetProblemResidual(&problem, *tac);
  ceres::Solve(*ceres_options_, &problem, &model->ceres_summary_); // Go !
  //    DD(model->ceres_summary_.FullReport());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::TimeIntegratedActivityImageFilter::
SelectBestModel(syd::FitModelBase::vector models,
                syd::TimeActivityCurve::pointer tac)
{
  DDF();
  bool verbose=true;
  int best = -1;
  double R2_threshold = options_.GetR2MinThreshold();
  double min_AICc = 666.0;
  double best_R2 = 0.0;
  for(auto i=0; i<models.size(); i++) {
    auto & m = models[i];
    double R2 = m->ComputeR2(tac);
    if (verbose) std::cout << m->GetName()
                           << " SS = " << m->ComputeRSS(tac)
                           << " R2 = " << R2;
    if (R2 > R2_threshold) {
      double AICc;
      bool b = m->IsAICcValid(tac->size()); //FIXME
      //if (b) AICc = m->ComputeAICc(tac);
      // else
      AICc = m->ComputeAIC(tac);
      //AICc = m->ComputeAICc(tac);
      if (verbose) std::cout << " " << b
                             << " AICc = " << m->ComputeAICc(tac)
                             << " AIC = " << m->ComputeAIC(tac);
      if (AICc < min_AICc) {
        best = i;
        min_AICc = AICc;
        best_R2 = R2;
      }
    }
  }
  return best;
}
// --------------------------------------------------------------------
