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
#include "sydTimeIntegratedActivityFilter.h"
#include "sydImageUtils.h"
#include "sydPrintTable.h"

// itk
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>
#include <itkNoiseImageFilter.h>
#include <itkLabelStatisticsImageFilter.h>

// --------------------------------------------------------------------
syd::TimeIntegratedActivityFilter::TimeIntegratedActivityFilter()
{
  SetLambdaPhysicHours(0.0);
  SetR2MinThreshold(0.9);
  SetRestrictedTACFlag(false);
  mask_ = 0;
  additional_point_flag_ = false;
  auto f2  = new syd::FitModel_f2;
  auto f3  = new syd::FitModel_f3;
  auto f4a = new syd::FitModel_f4a;
  auto f4b = new syd::FitModel_f4b;
  auto f4  = new syd::FitModel_f4;
  all_models_.push_back(f2);
  all_models_.push_back(f3);
  all_models_.push_back(f4a);
  all_models_.push_back(f4b);
  all_models_.push_back(f4);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFilter::SetModels(const std::vector<std::string> & model_names)
{
  int i=0;
  // Look for model from the given names
  for(auto & name:model_names) {
    bool b = false;
    for(auto model:all_models_) {
      if (model->GetName() == name) {
        AddModel(model, i+1); // start model id at 1 (such that 0 means no model)
        b = true;
        ++i;
      }
    }
    if (!b) {
      std::string km;
      for(auto model:all_models_) km += model->GetName()+" ";
      LOG(FATAL) << "Error the model '" << name << "' is not found. Known models are: " << km;
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::TimeIntegratedActivityFilter::GetRestrictedTac(syd::TimeActivityCurve::pointer initial_tac,
                                                          syd::TimeActivityCurve::pointer restricted_tac)
{
  restricted_tac->clear();

  // Select only the end of the curve (min 2 points);
  auto first_index = initial_tac->FindMaxIndex();
  first_index = std::min(first_index, initial_tac->size()-3);
  for(auto i=first_index; i<initial_tac->size(); i++) {
    restricted_tac->AddValue(initial_tac->GetTime(i), initial_tac->GetValue(i));
  }
  return first_index;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFilter::CreateIntegratedActivity(syd::TimeActivityCurve::pointer initial_tac)
{
  // Init solver
  InitSolver();

  // restricted tac ?
  syd::TimeActivityCurve::pointer working_tac;
  int index = 0;
  if (restricted_tac_flag_) {
    working_tac = syd::TimeActivityCurve::New();
    index = GetRestrictedTac(initial_tac, working_tac);
  }
  else working_tac = initial_tac;

  for(auto o:outputs_) {
    o->initial_tac_ = initial_tac;
    o->working_tac_ = working_tac;
  }

  // Solve
  int best;
  best = FitModels(working_tac);

  if (best != -1) {
    current_model_ = models_[best];
    if (restricted_tac_flag_) auc_output_->index_ = index;
    for(auto o:outputs_) { // Update the outputs
      o->model_ = current_model_;
      o->Update();
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFilter::CreateIntegratedActivityImage()
{
  // typedef
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;

  // Initialisation
  tac_image_ = Image4DType::New();
  InitInputData();

  // create initial tac with the times
  syd::TimeActivityCurve::pointer initial_tac = syd::TimeActivityCurve::New();
  for(auto t:times_) initial_tac->AddValue(t, 0.0);

  // restricted tac ?
  syd::TimeActivityCurve::pointer working_tac;
  if (restricted_tac_flag_) {
    working_tac = syd::TimeActivityCurve::New();
  }
  else working_tac = initial_tac;

  // Init solver
  InitSolver();

  // Init main iterator
  typedef itk::ImageRegionIterator<Image4DType> Iterator4D;
  Iterator4D it(tac_image_, tac_image_->GetLargestPossibleRegion());

  // Init mask iterator
  bool mask_flag = false;
  Iterator3D it_mask;
  if (mask_) {
    mask_flag = true;
    it_mask = Iterator3D(mask_, mask_->GetLargestPossibleRegion());
    it_mask.GoToBegin();
  }

  // Init output iterators
  for(auto & o:outputs_) {
    o->InitImage(images_[0]); // allocate images
    o->iterator.GoToBegin();
    o->initial_tac_ = initial_tac;
    o->working_tac_ = working_tac;;
  }

  // main loop
  int x = 0;
  int n = images_[0]->GetLargestPossibleRegion().GetNumberOfPixels();
  int number_of_pixels_in_mask = 0;
  int number_of_pixels_success = 0;
  for (it.GoToBegin(); !it.IsAtEnd(); ) {

    // Check if pixel is in the mask
    if (mask_flag and it_mask.Get() == 0) { // skip it
      for(auto i=0; i<images_.size(); i++) ++it;
    }
    else {
      ++number_of_pixels_in_mask;
      // Create current tac
      for(auto i=0; i<images_.size(); i++) {
        initial_tac->SetValue(i, it.Get());
        ++it; // next value
      }

      /*
      if (additional_point_flag_) {
        int n = images_.size();
        double last_value = tac.GetValue(n-1);
        double value = last_value * additional_point_value_;
        double time = tac.GetTime(n-1) + additional_point_time_;
        if (tac.size() == n) {
          tac.AddValue(time, value);
        }
        else {
          tac.SetTime(n, time);
          tac.SetValue(n, value);
        }
      }
      */
      int index = 0;
      if (restricted_tac_flag_) {
        index = GetRestrictedTac(initial_tac, working_tac);
        auc_output_->index_ = index; // needed to compute with paralelogram method
      }

      // Solve
      int best = FitModels(working_tac);

      // Set the current selected model, update the output
      if (best != -1) {
        ++number_of_pixels_success;
        current_model_ = models_[best];
        // Update output
        for(auto o:outputs_) {
          o->model_ = current_model_;
          o->Update();
        }
      }
    }

    // Next debug images
    for(auto o:outputs_) ++o->iterator;

    // progress bar
    ++x;

    if (sydlog::Log::LogLevel() > 0) loadbar(x,n);

    // Next in mask
    if (mask_flag) ++it_mask;
  }

  LOG(2) << "Done. " << x << " pixels  " << number_of_pixels_in_mask << " pixels in mask  "
         << number_of_pixels_success << " successfully fit  "
         << number_of_pixels_in_mask-number_of_pixels_success << " fail";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::TimeIntegratedActivityFilter::FitModels(syd::TimeActivityCurve::pointer tac)
{
  for(auto model:models_) {
    ceres::Problem problem;// New problem each time. (I did not manage to change that)
    model->SetProblemResidual(&problem, *tac);
    ceres::Solve(*ceres_options_, &problem, &model->ceres_summary_); // Go !
  }

  // Select the best model
  int best = -1;
  double R2_threshold = R2_min_threshold_;
  double best_AICc = 666;
  double best_R2 = 0.0;
  for(auto i=0; i<models_.size(); i++) {
    auto & m = models_[i];
    if (m->IsAcceptable()) {
      double R2 = m->ComputeR2(tac);
      if (R2 > R2_threshold and R2 > best_R2) {
        double AICc;
        bool b = m->IsAICcValid(tac->size());
        if (b) AICc = m->ComputeAICc(tac);
        if (!b or AICc < best_AICc) { // if AICc not valid, consider it is ok
          best = i;
          best_AICc = AICc;
          best_R2 = R2;
        }
      }
    }
  }
  return best;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFilter::InitInputData()
{
  // Check image size
  bool b = true;
  for(auto image:images_) b = b and syd::CheckImageSameSizeAndSpacing<ImageType::ImageDimension>(images_[0], image);
  if (!b) {
    LOG(FATAL) << "The images must have the same size/spacing, abort.";
  }

  // Sort images by time.
  // http://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
  // First, get the index of sorted times
  std::vector<size_t> idx(images_.size());
  for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;
  std::vector<double> t = times_;
  std::sort(idx.begin(), idx.end(), [t](size_t i1, size_t i2) {return t[i1] < t[i2];});
  // Sort the times
  std::sort(times_.begin(), times_.end());
  // Sort the images
  std::vector<ImageType::Pointer> temp(images_.size());
  for(auto i=0; i<idx.size(); i++) temp[i] = images_[idx[i]];
  for(auto i=0; i<idx.size(); i++) images_[i] = temp[i];

  // consider images_ OR create a 4D images ?
  typename Image4DType::SizeType size;
  for(auto i=1; i<4; i++) size[i] = images_[0]->GetLargestPossibleRegion().GetSize()[i-1];
  size[0] = images_.size();
  typename Image4DType::RegionType region;
  region.SetSize(size);
  tac_image_->SetRegions(region);
  typename Image4DType::SpacingType spacing;
  for(auto i=1; i<4; i++) spacing[i] = images_[0]->GetSpacing()[i-1];
  spacing[0] = 1.0;
  tac_image_->SetSpacing(spacing);
  typename Image4DType::PointType origin;
  for(auto i=1; i<4; i++) origin[i] = images_[0]->GetOrigin()[i-1];
  origin[0] = 0.0;
  tac_image_->SetOrigin(origin);
  tac_image_->Allocate();

  // Copy data to the 4D image
  Iterator4D it(tac_image_, tac_image_->GetLargestPossibleRegion());
  std::vector<Iterator3D> iterators;
  for(auto image:images_) {
    iterators.push_back(Iterator3D(image, image->GetLargestPossibleRegion()));
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
  //syd::WriteImage<Image4DType>(tac_image_, "s4d.mhd");

  // Set the lambda_phys_hours_ for the models
  for(auto & m:models_) m->lambda_phys_hours_ = image_lambda_phys_in_hour_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFilter::InitSolver()
{
  // Solve
  ceres_options_ = new ceres::Solver::Options;
  ceres_options_->max_num_iterations = 50;
  ceres_options_->linear_solver_type = ceres::DENSE_QR; // because few parameters/data
  ceres_options_->minimizer_progress_to_stdout = false;
  ceres_options_->trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT; // LM is the default

  /*
  DD(ceres_options_->min_line_search_step_size);
  DD(ceres_options_->max_line_search_step_contraction);
  DD(ceres_options_->function_tolerance);
  DD(ceres_options_->parameter_tolerance);
  DD(ceres_options_->num_threads);
  */

  //ceres_options_->trust_region_strategy_type = ceres::DOGLEG;// (LM seems faster)
  //ceres_options_->dogleg_type = ceres::SUBSPACE_DOGLEG;
  ceres_options_->logging_type = ceres::SILENT;

  // Init the models
  for(auto m:models_) {
    m->SetLambdaPhysicHours(image_lambda_phys_in_hour_);
    // m->robust_scaling_ = robust_scaling_;
  }

  // Required output: auc, mask, integrated
  if (restricted_tac_flag_) {
    auc_output_ = new syd::FitOutputImage_AUC();
    AddOutputImage(auc_output_);
  }
  else {
    integrate_output_ = new syd::FitOutputImage_Integrate();
    AddOutputImage(integrate_output_);
  }
  success_output_ = new syd::FitOutputImage_Success();
  AddOutputImage(success_output_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFilter::AddModel(syd::FitModelBase * m, int id)
{
  auto p = std::find_if(models_.begin(), models_.end(), [&id](syd::FitModelBase * m)->bool { return m->id_ == id; });
  if (p != models_.end()) {
    LOG(FATAL) << "This id already exists in the list of model: " << id;
  }
  m->id_ = id;
  models_.push_back(m);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityFilter::SetAdditionalPoint(bool b, double time, double value)
{
  additional_point_flag_ = b;
  additional_point_time_ = time;
  additional_point_value_ = value;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage * syd::TimeIntegratedActivityFilter::GetOutput()
{
  if (restricted_tac_flag_)
    return auc_output_;
  else
    return integrate_output_;
}
// --------------------------------------------------------------------
