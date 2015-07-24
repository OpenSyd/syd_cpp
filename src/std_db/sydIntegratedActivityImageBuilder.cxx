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

// itk
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>

// --------------------------------------------------------------------
syd::IntegratedActivityImageBuilder::IntegratedActivityImageBuilder(syd::StandardDatabase * db)
{
  db_ = db;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::SetInput(syd::Image::vector & images)
{
  // Check sizes, order by date
  images_ = images;

  // Sort by acquisition date
  std::sort(begin(images_), end(images_),
            [images](syd::Image::pointer a, syd::Image::pointer b) {
              if (a->dicoms.size() == 0) return true;
              if (b->dicoms.size() == 0) return false;
              return a->dicoms[0]->acquisition_date < b->dicoms[0]->acquisition_date;
            });


}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::IntegratedActivityImageBuilder::GetOutput() const
{
  return output_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::SaveDebugPixel(const std::string & filename) const
{
  DD("SaveDebugPixel");
  syd::PrintTable ta;
  ta.AddColumn("time", 10, 4);
  for(auto d:debug_data) {
    ta.AddColumn(d.name, 10, 1);
  }
  ta.Init();
  for(auto t=0; t<current_tac_.size(); t++) {
    ta << current_tac_.GetTime(t);
    for(auto d:debug_data) ta << d.tac.GetValue(t);
    ta.Endl();
  }
  std::ofstream os(filename);
  ta.Print(os);
  os.close();
  DD("end debug pixel");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::SaveDebugModel(const std::string & filename) const
{
  DD("SaveDebugModel");
  // Create tac from the models
  std::vector<syd::TimeActivityCurve*> tacs;
  for(auto d:debug_data) {
    for(auto model:d.models) {
      syd::TimeActivityCurve * tac = model->GetTAC(0, current_tac_.GetTime(current_tac_.size()-1), 200);
      tacs.push_back(tac);
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
      std::cout << "\t" << model->GetName() << " " << d.summaries[i].BriefReport() << " "
                << p
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
  auto size = images_[0]->size;
  int index = z*size[0]*size[1] + y*size[0] + x;
  DebugType debug;
  debug.name = name;
  debug.x = x;
  debug.y = y;
  debug.z = z;
  debug.index = index;
  debug_data.push_back(debug);
  std::cout << "debug " << name << " " << x << " " << y << " " << z << std::endl;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::CreateIntegratedActivityImage()
{

  // FIXME separate itk only

  // alloc debug
  //debug_tac.resize(debug_pixels.size());

  // typedef
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;

  // Load itk images
  std::vector<ImageType::Pointer> itk_images;
  for(auto image:images_) {
    auto im = syd::ReadImage<ImageType>(db_->GetAbsolutePath(image));
    itk_images.push_back(im);
  }

  // consider images_ OR create a 4D images ?
  typedef itk::Image<PixelType,4> Image4DType;
  Image4DType::Pointer tac_image = Image4DType::New();
  typename Image4DType::SizeType size;
  for(auto i=1; i<4; i++) size[i] = itk_images[0]->GetLargestPossibleRegion().GetSize()[i-1];
  size[0] = itk_images.size();
  typename Image4DType::RegionType region;
  region.SetSize(size);
  tac_image->SetRegions(region);
  typename Image4DType::SpacingType spacing;
  for(auto i=1; i<4; i++) spacing[i] = itk_images[0]->GetSpacing()[i-1];
  spacing[0] = 1.0;
  tac_image->SetSpacing(spacing);
  typename Image4DType::PointType origin;
  for(auto i=1; i<4; i++) origin[i] = itk_images[0]->GetOrigin()[i-1];
  origin[0] = 0.0;
  tac_image->SetOrigin(origin);
  tac_image->Allocate();

  typedef itk::ImageRegionIterator<ImageType> Iterator3D;
  typedef itk::ImageRegionIterator<Image4DType> Iterator4D;
  Iterator4D it(tac_image, tac_image->GetLargestPossibleRegion());
  std::vector<Iterator3D> iterators;
  for(auto image:itk_images) {
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

  //WriteImage<Image4DType>(tac_image, "4D.mhd"); hard to see
  ImageType::Pointer itk_output = ImageType::New();
  itk_output->CopyInformation(itk_images[0]);
  itk_output->SetRegions(itk_images[0]->GetLargestPossibleRegion());
  itk_output->Allocate();
  Iterator3D out_iter(itk_output, itk_output->GetLargestPossibleRegion());

  // Calibration factor here ?

  // create output image both db+itk

  // create initial tac
  std::string starting_date = images_[0]->dicoms[0]->injection->date;
  for(auto image:images_) {
    double t = DateDifferenceInHours(image->dicoms[0]->acquisition_date, starting_date);
    current_tac_.AddValue(t, 0.0);
  }

  // Init solver
  InitSolver();
  nb_of_computed_pixels_ = 0;

  //double max_time = log(0.01)/(-models_[0]->GetLambdaPhysicHours()); // consider mono expo decay from max point
  //DD(max_time);
  current_tac_.AddValue(200, 0.0); // add final point
  current_tac_.AddValue(220, 0.0); // add final point
  current_tac_.AddValue(280, 0.0); // add final point
  current_tac_.AddValue(420, 0.0); // add final point

  current_tac_.AddValue(500, 0.0); // add final point

  // loop pixel ? list of iterators
  //  update values of the tac
  //  call tac integration
  //  check result
  //  set result to output image
  out_iter.GoToBegin();
  int index=0;
  for (it.GoToBegin(); !it.IsAtEnd(); ) {

    // Compute the tac
    for(auto i=0; i<itk_images.size(); i++) {
      current_tac_.SetValue(i, it.Get());
      ++it; // next value
    }
    // double max = current_tac_.GetValue(current_tac_.FindMaxIndex());
    // double max_time = log(0.01)/(-models_[0]->GetLambdaPhysicHours()); // consider mono expo decay from max point
    // // DD(max);
    // current_tac_.SetTime(itk_images.size(), max_time); //final point

    // Check for debug
    current_debug_flag_ = false;
    //    auto iter = std::find(debug_pixels.begin(), debug_pixels.end(), index);
    debug_current = std::find_if(debug_data.begin(), debug_data.end(),
                                 [index] (const DebugType & d) { return d.index == index; } );
    if (debug_current != debug_data.end()) {
      current_debug_flag_ = true;
      debug_current->tac = current_tac_;
    }

    // Integration
    double v = 0.0;
    if (debug_only_flag_) {
      if (current_debug_flag_) v = Integrate();
    }
    else {
      if (current_tac_.GetValue(0) > 500) v = Integrate();
    }
    //if (current_debug_flag_) v = Integrate();

    // Next
    out_iter.Set(v);
    ++out_iter;
    ++index;
  }

  // write result image
  WriteImage<ImageType>(itk_output, "debug.mhd");

  // update db (only once it is finished)

  DD("end");
  DD(nb_of_computed_pixels_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::IntegratedActivityImageBuilder::Integrate()
{
  nb_of_computed_pixels_++;

  for(auto model:models_) {

    ceres::Problem problem;// New problem each time ? to be changed FIXME
    model->SetProblemResidual(&problem, current_tac_);

    // Positivity constraints
    for(auto i=0; i<model->GetNumberOfParameters(); i++) {
      //      problem.SetParameterLowerBound(&model->GetParameters()[i], 0, 0); // A positive
    }

    ceres::Solve(*ceres_options_, &problem, &ceres_summary_);

    // std::cout << ceres_summary_.BriefReport() << "\n";
    // std::cout << ceres_summary_.FullReport() << "\n";

    // AICc_absolute
    // AICc_relative

    // Goodness of fit  = coefficient of determination = R^2 = 1 - SSre/SStot
    // SSre  = sum of squares of residuals
    // SStot =  total sum of squares (see Coefficient_of_determination wikipedia)

    // Correlation matrix ? Landaw [20] ?
    // http://ceres-solver.org/solving.html#covariance-estimation
    // Covariance covariance(options); etc

    // DEBUG
    if (current_debug_flag_) {
      syd::FitModelBase * m = model->Clone();
      debug_current->models.push_back(m);
      debug_current->summaries.push_back(ceres_summary_);
      // std::cout << ceres_summary_.BriefReport() << "\n";
      //std::cout << ceres_summary_.FullReport() << "\n";
      // DD(m->ComputeAUC());
    }

  }

  // Then : AICc_min, Delta_i, w_AICi
  // Selection ?

  int best_model = 0; // FIXME

  double r = models_[best_model]->ComputeAUC();

  if (current_debug_flag_) { DD(r); }

  /*

  // First point
  double t1 = 0;
  double t2 = current_tac_.GetTime(0);
  double A1 = 0;
  double A2 = current_tac_.GetValue(0);
  double r = (t2-t1)*(A2+A1)/2.0;//A1*t1 - t1*t1*(A1-A2)/(t1-t2)/2.0;

  // other points
  for(auto i=1; i<current_tac_.size(); i++) {
    t1 = current_tac_.GetTime(i-1);
    t2 = current_tac_.GetTime(i);
    A1 = current_tac_.GetValue(i-1);
    A2 = current_tac_.GetValue(i);
    // double a = tac_->GetValue(i);
    //double b = tac_->GetValue(i+1);
    //    double d = tac_->GetTime(i+1)-tac_->GetTime(i);
    r = r + (t2-t1)*(A2+A1)/2.0;
  }

  // Last point ? mono expo fit with physical half life only (?)
  */

  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::InitSolver()
{
  DD("InitSolver");

  // Solve
  ceres_options_ = new ceres::Solver::Options;
  ceres_options_->max_num_iterations = 50;
  ceres_options_->linear_solver_type = ceres::DENSE_QR; // because few parameters/data
  ceres_options_->minimizer_progress_to_stdout = false;
  ceres_options_->trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT; // LM is the default
  //ceres_options_->trust_region_strategy_type = ceres::DOGLEG;// (LM seems faster)
  //ceres_options_->dogleg_type = ceres::SUBSPACE_DOGLEG;
  ceres_options_->logging_type = ceres::SILENT;

  // Create the models
  models_.push_back(new syd::FitModel_f1);
  models_.push_back(new syd::FitModel_f2);
  //models_.push_back(new syd::FitModel_f3);
  models_.push_back(new syd::FitModel_f4a);
  //models_.push_back(new syd::FitModel_f4b);
  //models_.push_back(new syd::FitModel_f4c);
  models_.push_back(new syd::FitModel_f4);

  for(auto m:models_) {
    m->SetLambdaPhysicHours(0.010297405); // Indium in hour
    DD(robust_scaling_);
    m->robust_scaling_ = robust_scaling_;
  }

  DD("end InitSolver");
}
// --------------------------------------------------------------------
