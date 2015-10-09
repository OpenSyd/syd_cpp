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

// --------------------------------------------------------------------
syd::IntegratedActivityImageBuilder::IntegratedActivityImageBuilder()
{
  R2_min_threshold_ = 0.7;
  mask_ = 0;
  restricted_tac_flag_ = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::SaveDebugPixel(const std::string & filename) const
{
  if (debug_data.size() < 1) return;

  syd::PrintTable ta;
  ta.AddColumn("time", 10, 4);
  for(auto d:debug_data) {
    ta.AddColumn(d.name, 10, 1);
  }
  ta.Init();
  auto tac = debug_data[0].tac;
  for(auto t=0; t<tac.size(); t++) {
    ta << tac.GetTime(t);
    for(auto d:debug_data) {
      ta << d.tac.GetValue(t);
    }
    ta.Endl();
  }
  std::ofstream os(filename);
  ta.Print(os);
  os.close();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::SaveDebugModel(const std::string & filename)
{
  if (debug_data.size() < 1) return;

  for(auto & d:debug_data) {
    for(auto model:d.models) {
      syd::FitModelBase * subm = model->Clone();
      subm->lambda_phys_hours_ = 0.0043449876; // 177Lu
      //subm->lambda_phys_hours_ = 0.010823439; // 90Y
      subm->name_ = subm->name_+"177Lu";
      d.models.push_back(subm);
    }
  }


  // Create tac from the models
  auto tac = debug_data[0].tac;
  std::vector<syd::TimeActivityCurve*> tacs;
  for(auto d:debug_data) {
    for(auto model:d.models) {
      syd::TimeActivityCurve * t = model->GetTAC(0, tac.GetTime(tac.size()-1), 200);
      tacs.push_back(t);
    }
  }

  // Set table column name
  syd::PrintTable ta;
  ta.AddColumn("time", 10, 4);
  for(auto d:debug_data) {
    for(auto model:d.models) {
      std::string name = d.name+"_"+model->GetName();
      ta.AddColumn(name, 20,1);
    }
  }
  ta.Init();

  // Set values
  for(auto t=0; t<tacs[0]->size(); t++) {
    ta << tacs[0]->GetTime(t);
    for(auto tac:tacs) ta << tac->GetValue(t);
    ta.Endl();
  }

  // Dump
  std::ofstream os(filename);
  ta.Print(os);
  os.close();

  // Other debug
  for(auto d:debug_data) {
    std::cout << d.name << " "  << std::endl;
    for(auto i=0; i< d.models.size(); i++) {
      auto model = d.models[i];
      std::string p;
      for(auto pp:model->GetParameters()) p = p+syd::ToString(pp)+" ";
      std::cout << " " << model->GetName() << " " //<< model->ceres_summary_.BriefReport() << " "
                << p
                << (i == d.selected_model ? "**Selected**":"")
                << " R2 = " << model->ComputeR2(d.tac, restricted_tac_flag_)
                << " AICc = " << model->ComputeAICc(d.tac)
                << " AUC = " << model->ComputeAUC(d.tac, model->lambda_phys_hours_, restricted_tac_flag_)
                << " l = " << model->lambda_phys_hours_
                << std::endl;
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::AddDebugPixel(std::string name, int x, int y, int z)
{
  if (images_.size() == 0) {
    LOG(FATAL) << "Only use AddDebugPixel after SetInput";
  }
  auto size = images_[0]->GetLargestPossibleRegion().GetSize();
  int index = z*size[0]*size[1] + y*size[0] + x;
  DebugType debug;
  debug.name = name;
  debug.x = x;
  debug.y = y;
  debug.z = z;
  debug.index = index;
  debug_data.push_back(debug);
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
    o->iterator.GoToBegin();
    o->use_current_tac = restricted_tac_flag_;
  }

  // debug init, sort point by index
  bool debug_this_point_flag;
  int debug_point_current=0;
  std::sort(begin(debug_data), end(debug_data),
            [](DebugType a, DebugType b) {
              return a.index < b.index; });
  if(debug_data.size() == 0) debug_point_current = -1;

  // log
  //  models, threshold, mask
  std::string sm;
  for(auto m:models_) sm += m->name_+" ";
  LOG(1) << "Starting fit with models : " << sm << "; "
         << (mask_flag ? "with mask":"no_mask")
         << " ; R2_min = " << R2_min_threshold_
         << (restricted_tac_flag_ ? " ; fit with last 3 points of the curve only":"; fit on all timepoints");

  // main loop
  int x = 0;
  int n = images_[0]->GetLargestPossibleRegion().GetNumberOfPixels();
  int number_of_pixels_in_mask = 0;
  int number_of_pixels_success = 0;
  for (it.GoToBegin(); !it.IsAtEnd(); ) {

    // Consider current point, is it a debug point ?
    if (debug_point_current != -1 and x == debug_data[debug_point_current].index) debug_this_point_flag = true;
    else debug_this_point_flag = false;

    // Check if pixel is in the mask
    if (((mask_flag and it_mask.Get() == 0) and !debug_this_point_flag)
        or
        (debug_only_flag_ and !debug_this_point_flag)) { // skip it
      for(auto i=0; i<images_.size(); i++) ++it;
    }
    else {
      ++number_of_pixels_in_mask;
      // Create current tac
      for(auto i=0; i<images_.size(); i++) {
        tac.SetValue(i, it.Get());
        ++it; // next value
      }

      syd::TimeActivityCurve restricted_tac;
      if (restricted_tac_flag_) {
        // Select only the end of the curve (min 2 points);
        auto m = tac.FindMaxIndex();
        m = std::min(m, tac.size()-2);
        for(auto i=m; i<tac.size(); i++)
          restricted_tac.AddValue(tac.GetTime(i), tac.GetValue(i));
      }

      // Solve
      int best;
      if (restricted_tac_flag_)
        best = FitModels(restricted_tac, debug_this_point_flag, &debug_data[debug_point_current]);
      else
        best = FitModels(tac, debug_this_point_flag, &debug_data[debug_point_current]);

      // Set the current selected model, update the output
      if (best != -1) {
        ++number_of_pixels_success;
        current_model_ = models_[best];
        // Update output
        for(auto o:outputs_) o->Update(tac, current_model_);
      }

      // debug points for plot
      if (debug_this_point_flag) {
        debug_data[debug_point_current].selected_model = best;
        debug_data[debug_point_current].tac = tac;
        ++debug_point_current;
      }
    }

    // Next debug images
    for(auto o:outputs_) ++o->iterator;

    // progress bar
    ++x;
    loadbar(x,n);

    // Next in mask
    if (mask_flag) ++it_mask;
  }

  LOG(2) << "Done. " << x << " pixels  " << number_of_pixels_in_mask << " pixels in mask  "
         << number_of_pixels_success << " successfully fit  "
         << number_of_pixels_in_mask-number_of_pixels_success << " fail";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::IntegratedActivityImageBuilder::FitModels(TimeActivityCurve & tac,
                                                   bool debug_this_point_flag,
                                                   DebugType * debug_current)
{
  for(auto model:models_) {
    ceres::Problem problem;// New problem each time. (I did not manage to change that)
    model->SetProblemResidual(&problem, tac);
    ceres::Solve(*ceres_options_, &problem, &model->ceres_summary_); // Go !

    if (debug_this_point_flag) {
      syd::FitModelBase * mm = model->Clone();
      debug_current->models.push_back(mm);
      // Copy the current tac
      syd::TimeActivityCurve * c = new TimeActivityCurve;
      c->CopyFrom(tac);
      mm->current_tac = c;
    }
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
  for(auto image:images_) b = b and syd::CheckImageSameSizeAndSpacing<ImageType>(images_[0], image);
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
  syd::WriteImage<Image4DType>(tac_image_, "s4d.mhd");

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
