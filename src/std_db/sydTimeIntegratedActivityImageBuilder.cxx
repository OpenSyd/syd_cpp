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

// --------------------------------------------------------------------
syd::TimeIntegratedActivityImageBuilder::
TimeIntegratedActivityImageBuilder()
{
  min_activity_ = 0.0;
  debug_images_flag_ = false;

  // Create FitOutputImage
  auc = std::make_shared<syd::FitOutputImage_AUC>();
  integrate = std::make_shared<syd::FitOutputImage_Integrate>();
  r2 = std::make_shared<syd::FitOutputImage_R2>();
  best_model = std::make_shared<syd::FitOutputImage_Model>();
  iter = std::make_shared<syd::FitOutputImage_Iteration>();
  success = std::make_shared<syd::FitOutputImage_Success>();
  all_outputs_.push_back(auc);
  all_outputs_.push_back(integrate);
  all_outputs_.push_back(r2);
  all_outputs_.push_back(best_model);
  all_outputs_.push_back(iter);
  all_outputs_.push_back(success);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::
SetInput(const syd::Image::vector images)
{
  inputs_ = images;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::
SetImageActivityThreshold(const double min_activity)
{
  min_activity_ = min_activity;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::
SetDebugOutputFlag(bool debug)
{
  debug_images_flag_ = debug;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::
SetOptions(syd::TimeIntegratedActivityFitOptions options)
{
  options_ = options;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::
WriteOutput()
{
  auto outputs = filter_.GetOutputs();
  for(auto output:outputs) output->WriteImage();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::
InsertOutputImagesInDB()
{
  DDF();
  DD("TODO");
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::
Run()
{
  DDF();

  // Check input data
  CheckInputs();

  // Sort images
  auto db = inputs_[0]->GetDatabase<syd::StandardDatabase>();
  db->Sort<syd::Image>(inputs_);

  // Get itk image
  std::vector<ImageType::Pointer> itk_images;
  std::vector<double> times;
  for(auto input:inputs_) {
    itk_images.push_back(syd::ReadImage<ImageType>(input->GetAbsolutePath()));
    times.push_back(input->GetHoursFromInjection());
  }
  DDS(times);

  // Build mask
  auto mask = CreateMaskFromThreshold(itk_images, min_activity_);
  int nb_pixels = syd::ComputeSumOfPixelValues<MaskImageType>(mask);
  DD(nb_pixels);

  // set filter info
  filter_.ClearInput();
  for(auto i=0; i<times.size(); i++)
    filter_.AddInput(itk_images[i], times[i]);
  filter_.SetLambdaDecayConstantInHours(inputs_[0]->injection->GetLambdaDecayConstantInHours());
  filter_.SetMask(mask);
  if (options_.GetRestrictedFlag()) filter_.AddOutputImage(auc);
  else filter_.AddOutputImage(integrate);
  if (debug_images_flag_) {
    filter_.AddOutputImage(r2);
    filter_.AddOutputImage(success);
    filter_.AddOutputImage(best_model);
    filter_.AddOutputImage(iter);
  }
  filter_.SetOptions(options_);

  // Print
  std::string sm;
  filter_.InitModels(); // require to get the model names
  auto models = filter_.GetModels();
  for(auto m:models) sm += m->GetName()+" ("+std::to_string(m->GetId())+") ";
  LOG(2) << "Starting fit: models= " << sm << "; "
         << (min_activity_>0.0 ? "with mask":"no_mask")
         << " pixels=" << nb_pixels
         << " R2_min= " << options_.GetR2MinThreshold()
         << (options_.GetRestrictedFlag() ? " restricted":" non restricted");

  // Go !
  filter_.Run();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::
CheckInputs()
{
  DDF();
  if (inputs_.size() < 2) {
    EXCEPTION("Error at least 2 images needed");
  }

  // Check injection
  syd::Injection::pointer injection = inputs_[0]->injection;
  bool b = true;
  for(auto image:inputs_) {
    b = b and (injection->id == image->injection->id);
  }
  if (!b) {
    EXCEPTION("The image do not have the same injection.");
  }
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
syd::TimeIntegratedActivityImageBuilder::MaskImageType::Pointer
syd::TimeIntegratedActivityImageBuilder::
CreateMaskFromThreshold(std::vector<ImageType::Pointer> images,
                        double min_activity)
{
  DDF();

  // Create image with MIN value along the sequence
  ImageType::Pointer first_image = images[0];
  MaskImageType::Pointer mask = syd::CreateImageLike<MaskImageType>(first_image);
  std::vector<Iterator> it;
  for(auto image:images) {
    Iterator i = Iterator(image, image->GetLargestPossibleRegion());
    i.GoToBegin();
    it.push_back(i);
  }
  MaskIterator it_mask(mask, mask->GetLargestPossibleRegion());
  while (!it_mask.IsAtEnd()) {
    auto mini = it[0].Get();
    for(auto & itt:it) mini = std::min(mini, itt.Get());
    if (mini < min_activity_) it_mask.Set(0);
    else it_mask.Set(1);
    ++it_mask;
    for(auto & itt:it) ++itt;
  }

  syd::WriteImage<MaskImageType>(mask, "mask.mhd");
  return mask;
}
// --------------------------------------------------------------------
