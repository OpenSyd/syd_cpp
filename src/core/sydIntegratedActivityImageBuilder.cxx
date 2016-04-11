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
#include "sydIntegratedActivityImageBuilder.h"
#include "sydImageUtils.h"
#include "sydPrintTable.h"

// itk
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>
#include <itkNoiseImageFilter.h>
#include <itkLabelStatisticsImageFilter.h>

// --------------------------------------------------------------------
syd::IntegratedActivityImageBuilder::IntegratedActivityImageBuilder()
{
  SetLambdaPhysicHours(0.0);
  SetR2MinThreshold(0.9);
  SetRestrictedTACFlag(false);
  mask_ = 0;
  additional_point_flag_ = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::CreateIntegratedActivityInROI()
{
  // auto output
  auc_output_ = new syd::FitOutputImage_AUC(image_lambda_phys_in_hour_);
  success_output_ = new syd::FitOutputImage_Success();
  AddOutputImage(auc_output_);
  AddOutputImage(success_output_);

  // Init solver
  InitSolver();

  // Solve
  int best;
  best = FitModels(tac_);

  if (best != -1) {
    current_model_ = models_[best];
    DD(current_model_->GetName());
    for(auto o:outputs_) { // Update the outputs
      o->Update(tac_, tac_, current_model_);
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::CreateIntegratedActivityImage()
{
  // typedef
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;

  // Initialisation
  tac_image_ = Image4DType::New();
  InitInputData();

  // create initial tac with the times
  TimeActivityCurve tac;
  for(auto t:times_) tac.AddValue(t, 0.0);

  // Init solver
  InitSolver();

  // Init main iterator
  typedef itk::ImageRegionIterator<Image4DType> Iterator4D;
  Iterator4D it(tac_image_, tac_image_->GetLargestPossibleRegion());

  // Required output: auc and mask
  auc_output_ = new syd::FitOutputImage_AUC(image_lambda_phys_in_hour_);
  success_output_ = new syd::FitOutputImage_Success();
  AddOutputImage(auc_output_);
  AddOutputImage(success_output_);

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
    o->use_current_tac = restricted_tac_flag_;
  }

  // log
  //  models, threshold, mask
  std::string sm;
  for(auto m:models_) sm += m->name_+" ("+syd::ToString(m->id_)+") ";
  LOG(2) << "Starting fit with models : " << sm << "; "
         << (mask_flag ? "with mask":"no_mask")
         << " ; R2_min = " << R2_min_threshold_
         << (restricted_tac_flag_ ? " ; fit with last points of the curve (from max value)":"; fit on all timepoints");

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
        tac.SetValue(i, std::max((float)0.0, it.Get())); // FIXME
        ++it; // next value
      }

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

      syd::TimeActivityCurve restricted_tac;
      if (restricted_tac_flag_) {
        // Select only the end of the curve (min 2 points); FIXME ?
        auto m = tac.FindMaxIndex();
        m = std::min(m, tac.size()-3);
        for(auto i=m; i<tac.size(); i++)
          restricted_tac.AddValue(tac.GetTime(i), tac.GetValue(i));
      }

      // Solve
      int best;
      if (restricted_tac_flag_)
        best = FitModels(restricted_tac);
      else
        best = FitModels(tac);

      // Set the current selected model, update the output
      if (best != -1) {
        ++number_of_pixels_success;
        current_model_ = models_[best];
        // Update output
        for(auto o:outputs_) o->Update(tac, restricted_tac, current_model_);
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
int syd::IntegratedActivityImageBuilder::FitModels(TimeActivityCurve & tac)
{
  for(auto model:models_) {
    ceres::Problem problem;// New problem each time. (I did not manage to change that)
    model->SetProblemResidual(&problem, tac);
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
        bool b = m->IsAICcValid(tac.size());
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
void syd::IntegratedActivityImageBuilder::InitInputData()
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
  for(auto & iter:iterators) iter.GoToBegin();
  for (it.GoToBegin(); !it.IsAtEnd(); ) {
    for(auto & iter:iterators) {
      it.Set(iter.Get());
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
void syd::IntegratedActivityImageBuilder::InitSolver()
{
  // Solve
  ceres_options_ = new ceres::Solver::Options;
  ceres_options_->max_num_iterations = 50;
  ceres_options_->linear_solver_type = ceres::DENSE_QR; // because few parameters/data
  ceres_options_->minimizer_progress_to_stdout = false;
  ceres_options_->trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT; // LM is the default
  //ceres_options_->trust_region_strategy_type = ceres::DOGLEG;// (LM seems faster)
  //ceres_options_->dogleg_type = ceres::SUBSPACE_DOGLEG;
  ceres_options_->logging_type = ceres::SILENT;

  // Init the models
  for(auto m:models_) {
    m->SetLambdaPhysicHours(image_lambda_phys_in_hour_);
    // m->robust_scaling_ = robust_scaling_;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::AddModel(syd::FitModelBase * m, int id)
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
void syd::IntegratedActivityImageBuilder::SetAdditionalPoint(bool b, double time, double value)
{
  additional_point_flag_ = b;
  additional_point_time_ = time;
  additional_point_value_ = value;
}
// --------------------------------------------------------------------
