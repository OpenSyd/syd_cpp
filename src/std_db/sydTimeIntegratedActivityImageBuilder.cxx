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
#include "sydImageHelper.h"
#include "sydTagHelper.h"

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
WriteDebugOutput()
{
  auto outputs = filter_.GetOutputs();
  for(auto output:outputs) output->WriteImage();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::TimeIntegratedActivityImageBuilder::
InsertOutputImage()
{
  CheckInputs();
  // Get the first output: integrate or auc if restricted
  syd::FitOutputImage::pointer main_output = auc;
  if (options_.GetRestrictedFlag()) main_output = auc;
  else main_output = integrate;
  auto img = inputs_[0];
  // Create output image
  typedef syd::FitOutputImage::ImageType ImageType;
  auto output = syd::InsertImage<ImageType>(main_output->GetImage(), img->patient);
  syd::SetImageInfoFromImage(output, img);
  auto db = img->GetDatabase<syd::StandardDatabase>();
  if (output->pixel_unit == nullptr)
    output->pixel_unit = syd::FindOrCreatePixelUnit(db, "counts");
  auto unit = output->pixel_unit->name+".h";// get pixel unit times hours
  auto desc = unit+" times hours";
  output->pixel_unit = syd::FindOrCreatePixelUnit(db, unit, desc);
  db->Update(output);
  return output;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::vector syd::TimeIntegratedActivityImageBuilder::
InsertDebugOutputImages(std::vector<std::string> & names)
{
  //outputs_.clear();
  syd::Image::vector outputs;
  syd::FitOutputImage::pointer main_output = auc;
  if (options_.GetRestrictedFlag()) main_output = auc;
  else main_output = integrate;
  auto img = inputs_[0];
  auto db = img->GetDatabase<syd::StandardDatabase>();

  for(auto o:all_outputs_) {
    if (o->GetTagName() != main_output->GetTagName()) {
      auto output = syd::InsertImage<ImageType>(o->GetImage(), img->patient);
      syd::SetImageInfoFromImage(output, img);
      auto t = syd::FindOrCreateTag(db, o->GetTagName());
      syd::AddTag(output->tags, t);
      output->pixel_unit = syd::FindPixelUnit(db, "no_unit");
      db->Update(output);
      outputs.push_back(output);
      names.push_back(o->GetTagName());
    }
  }
  return outputs;
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
syd::TIA::pointer syd::TimeIntegratedActivityImageBuilder::
Run()
{
  // Check input data and get times
  auto times = CheckInputs();

  // Get itk image
  std::vector<ImageType::Pointer> itk_images;
  for(auto input:inputs_)
    itk_images.push_back(syd::ReadImage<ImageType>(input->GetAbsolutePath()));

  // Build mask
  auto mask = CreateMaskFromThreshold(itk_images, min_activity_);
  int nb_pixels = syd::ComputeSumOfPixelValues<MaskImageType>(mask);

  // set filter info
  filter_.ClearInput();
  for(auto i=0; i<times.size(); i++)
    filter_.AddInput(itk_images[i], times[i]);
  filter_.SetLambdaDecayConstantInHours(inputs_[0]->injection->GetLambdaDecayConstantInHours());
  filter_.SetMask(mask);
  if (options_.GetRestrictedFlag()) filter_.AddOutputImage(auc);
  else filter_.AddOutputImage(integrate);
  if (debug_images_flag_) {
    if (options_.GetRestrictedFlag()) filter_.AddOutputImage(integrate);
    else filter_.AddOutputImage(auc);
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
  for(auto m:models) sm += m->GetName()+"("+std::to_string(m->GetId())+") ";

  LOG(1) << "Starting fit: "
         << "t= " << syd::ArrayToString(filter_.GetTimes(), 2)
         << " ; m= " << sm << "; "
         << (min_activity_>0.0 ? "with mask":"no_mask")
         << " ; pixels= " << nb_pixels
         << " ; R2_min= " << options_.GetR2MinThreshold() << " ; "
         << (options_.GetRestrictedFlag() ? "restricted":"non restricted")
         << " ; " << options_.GetAkaikeCriterion();

  // Go !
  filter_.Run();

  // Create output
  auto db = inputs_[0]->GetDatabase<syd::StandardDatabase>();
  syd::TIA::pointer tia;
  db->New(tia);

  for(auto in:inputs_) tia->images.push_back(in);
  tia->min_activity = min_activity_;
  tia->r2_min = options_.GetR2MinThreshold();
  tia->max_iteration = options_.GetMaxNumIterations();
  tia->restricted_tac = options_.GetRestrictedFlag();
  tia->models_name.clear();
  for(auto m:options_.GetModels())
    tia->models_name.push_back(m->GetName());
  tia->nb_pixels = GetFilter().GetNumberOfPixels();
  tia->nb_success_pixels = GetFilter().GetNumberOfSuccessfullyFitPixels();

  // Outputs
  auto output = InsertOutputImage();
  tia->AddOutput(output, "auc");
  syd::FitOutputImage::pointer main_output = auc;
  if (options_.GetRestrictedFlag()) main_output = auc;
  else main_output = integrate;
  if (debug_images_flag_) {
    std::vector<std::string> names;
    auto outputs = InsertDebugOutputImages(names);
    for(auto i=0; i<outputs.size(); i++) {
      tia->AddOutput(outputs[i], names[i]);
    }
  }
  return tia;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::vector<double> syd::TimeIntegratedActivityImageBuilder::
CheckInputs()
{
  if (inputs_.size() < 2) {
    EXCEPTION("Error at least 2 images needed");
  }

  // Check injection
  syd::Injection::pointer injection = inputs_[0]->injection;
  bool b = true;
  for(auto image:inputs_) {
    if (image->injection == nullptr) {
      EXCEPTION("Images need to be associated with injection.");
    }
    b = b and (injection->id == image->injection->id);
  }
  if (!b) {
    EXCEPTION("The image do not have the same injection.");
  }

  // Sort images
  auto db = inputs_[0]->GetDatabase<syd::StandardDatabase>();
  db->Sort<syd::Image>(inputs_);

  // Get times
  std::vector<double> times;
  for(auto input:inputs_) times.push_back(input->GetHoursFromInjection());
  auto tiny = 0.0001;
  for(auto i=1; i<times.size(); i++)
    if (fabs(times[i] - times[i-1]) < tiny)
      EXCEPTION("Error with initial input image times: too close ?");
  return times;
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
syd::TimeIntegratedActivityImageBuilder::MaskImageType::Pointer
syd::TimeIntegratedActivityImageBuilder::
CreateMaskFromThreshold(std::vector<ImageType::Pointer> images,
                        double min_activity)
{
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
    auto maxi = it[0].Get();
    for(auto & itt:it) maxi = std::max(maxi, itt.Get());
    if (maxi < min_activity_) it_mask.Set(0);
    else it_mask.Set(1);
    ++it_mask;
    for(auto & itt:it) ++itt;
  }
  return mask;
}
// --------------------------------------------------------------------

