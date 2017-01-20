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
syd::TimeIntegratedActivityImageFilter::TimeIntegratedActivityImageFilter():
  TimeIntegratedActivityFilter()
{
  mask_ = nullptr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeIntegratedActivityImageFilter::~TimeIntegratedActivityImageFilter()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::Run()
{
  // Check inputs: size, times, negative values ?
  options_.Check();
  CheckInputs();

  // Initialisation: mask (create if do not exist)
  InitMask();

  // Initialisation: create 4D images
  Init4DInput();

  // Initialisation TAC
  initial_tac_ = syd::TimeActivityCurve::New();
  for(auto t:times_) initial_tac_->AddValue(t, 0);
  working_tac_ = syd::TimeActivityCurve::New();
  //  DD("ADD value here");//FIXME

  // Initialisation: FitOutputImage
  InitOutputs();

  // Initialisation: Models
  models_ = options_.GetModels();

  // Initialisation: Solver
  InitSolver();

  // Initialisation: iterators (both input and output)
  Iterator4D it(tac_image_, tac_image_->GetLargestPossibleRegion());
  MaskIterator it_mask(mask_, mask_->GetLargestPossibleRegion());
  it_mask.GoToBegin();
  for(auto & o:outputs_) o->iterator.GoToBegin();

  // Main loop
  int x = 0;
  //  int n = images_[0]->GetLargestPossibleRegion().GetNumberOfPixels();
  int n = mask_->GetLargestPossibleRegion().GetNumberOfPixels();
  nb_pixels_ = 0;
  nb_successful_fit_ = 0;
  for (it.GoToBegin(); !it.IsAtEnd(); ) {
    if (it_mask.Get() != 0) { // inside the mask
      ++nb_pixels_;
      auto best_model_index = FitOnePixel(it);
      if (best_model_index != -1) { // success
        auto best_model = models_[best_model_index];
        ++nb_successful_fit_;
        for(auto & o:outputs_) {
          o->SetInitialTimeActivityCurve(initial_tac_);
          o->SetWorkingTimeActivityCurve(working_tac_);
          o->Update(best_model);
        }
      }
    }
    else {
      // FIXME update output ?
    }

    // progress bar
    ++x;
    if (sydlog::Log::LogLevel() > 0) loadbar(x,n);

    // Next pixel for output
    for(auto o:outputs_) o->Iterate();
    // Next pixel for mask
    ++it_mask;
    // Next pixel for tac image (skip n values)
    for(auto i=0; i<images_.size(); i++) ++it;
    //    ++it;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::TimeIntegratedActivityImageFilter::FitOnePixel(Iterator4D it)
{
  // Create initial tac
  for(auto i=0; i<images_.size(); i++) {
    initial_tac_->SetValue(i, it.Get());
    ++it; // next value
  }

  // FIXME: todo add value

  // Create working tac (restricted, + add value)
  if (options_.GetRestrictedFlag()) {
    if (initial_tac_ == working_tac_)  {
      // special case when call FitOnePixel ouside the loop
      // (syd_test6)
      working_tac_ = syd::TimeActivityCurve::New();
    }
    GetRestrictedTac(initial_tac_, working_tac_);
  }
  else working_tac_ = initial_tac_;

  // Loop on models
  for(auto model:models_) {
    FitTACWithModel(model, working_tac_);
  }

  // Select best one
  return SelectBestModel(models_, working_tac_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::InitOutputs()
{
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
  // Create
  tac_image_ = Image4DType::New();

  // Create a 4D images with the set of 3D images
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
  //  syd::WriteImage<Image4DType>(tac_image_, "s4d.mhd");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::InitMask()
{
  if (mask_ == nullptr) { // create mask same size
    mask_ = syd::CreateImageLike<MaskImageType>(images_[0]);
    mask_->Allocate();
    mask_->FillBuffer(1.0); // every pixels
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::CheckInputs()
{
  if (images_.size() < 2) {
    EXCEPTION("Provide at least 2 images before");
  }

  // Check image size
  bool b = true;
  for(auto image:images_)
    b = b and syd::ImagesHaveSameSupport<ImageType, ImageType>(images_[0], image);
  if (mask_ != nullptr)
    b = b and syd::ImagesHaveSameSupport<ImageType, MaskImageType>(images_[0], mask_);
  if (!b) {
    EXCEPTION("The images + mask must have the same size/spacing, abort.");
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::AddInput(ImageType::Pointer image, double time)
{
  images_.push_back(image);
  times_.push_back(time);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageFilter::
AddOutputImage(syd::FitOutputImage::pointer o)
{
  outputs_.push_back(o);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeIntegratedActivityImageFilter::Iterator4D
syd::TimeIntegratedActivityImageFilter::
GetIteratorAtPoint(double x, double y, double z)
{
  Image4DType::PointType point;
  point[0] = 0;
  point[1] = x;
  point[2] = y;
  point[3] = z;
  Image4DType::IndexType index;
  tac_image_->TransformPhysicalPointToIndex(point, index);
  Iterator4D it(tac_image_, tac_image_->GetLargestPossibleRegion());
  it.SetIndex(index);
  return it;
}
// --------------------------------------------------------------------
