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
#include "sydImageUtils.h"
#include "sydImageAnd.h"
#include "sydRoiMaskImageHelper.h"
#include "sydImageCrop.h"

// --------------------------------------------------------------------
syd::TimeIntegratedActivityImageBuilder::
TimeIntegratedActivityImageBuilder()
{
  min_activity_ = 0.0;
  debug_images_flag_ = false;
  additional_mask_name_ = "body";

  // Create FitOutputImage
  auc = std::make_shared<syd::FitOutputImage_AUC>();
  integrate = std::make_shared<syd::FitOutputImage_Integrate>();
  r2 = std::make_shared<syd::FitOutputImage_R2>();
  best_model = std::make_shared<syd::FitOutputImage_Model>();
  iter = std::make_shared<syd::FitOutputImage_Iteration>();
  success = std::make_shared<syd::FitOutputImage_Success>();
  params = std::make_shared<syd::FitOutputImage_ModelParams>();
  mrt = std::make_shared<syd::FitOutputImage_MRT>();
  all_outputs_.push_back(auc);
  all_outputs_.push_back(integrate);
  all_outputs_.push_back(r2);
  all_outputs_.push_back(best_model);
  all_outputs_.push_back(iter);
  all_outputs_.push_back(success);
  all_outputs_.push_back(params);
  all_outputs_.push_back(mrt);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedActivityImageBuilder::
SetInput(const syd::Image::vector images)
{
  images_ = images;
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
InsertOutputAUCImage()
{
  CheckInputs();
  // Get the first output: integrate or auc if restricted
  syd::FitOutputImage::pointer main_output = auc;
  if (options_.GetRestrictedFlag()) main_output = auc;
  else main_output = integrate;
  auto img = images_[0];
  // Create output image
  typedef syd::FitOutputImage::ImageType ImageType;
  auto output = syd::InsertImage<ImageType>(main_output->GetImage(), img->patient);
  syd::SetImageInfoFromImage(output, img);
  output->tags.clear(); // remove all tags
  auto db = img->GetDatabase<syd::StandardDatabase>();
  if (output->pixel_unit == nullptr)
    output->pixel_unit = syd::FindOrCreatePixelUnit(db, "counts");
  auto unit = output->pixel_unit->name+".h";// get pixel unit times hours
  auto desc = unit+" times hours";
  output->pixel_unit = syd::FindOrCreatePixelUnit(db, unit, desc);
  auto t = syd::FindOrCreateTag(db, main_output->GetTagName());
  syd::AddTag(output->tags, t);
  db->Update(output);
  return output;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::TimeIntegratedActivityImageBuilder::
InsertOutputSuccessFitImage(typename MaskImageType::Pointer mask_itk)
{
  CheckInputs();
  auto img = images_[0];
  auto success_img_itk = success->GetImage();//syd::ReadImage<ImageType>(img->GetAbsolutePath());

  // Modify the image according to the mask (put -1)
  MaskIterator it_mask(mask_itk, mask_itk->GetLargestPossibleRegion());
  Iterator it_img(success_img_itk, success_img_itk->GetLargestPossibleRegion());
  it_mask.GoToBegin();
  it_img.GoToBegin();
  while (!it_mask.IsAtEnd()) {
    if (it_mask.Get() == 0) it_img.Set(-1);
    ++it_mask;
    ++it_img;
  }

  // Create output image
  typedef syd::FitOutputImage::ImageType ImageType;
  auto output = syd::InsertImage<ImageType>(success_img_itk, img->patient);
  syd::SetImageInfoFromImage(output, img);
  output->tags.clear(); // remove all tags
  auto db = img->GetDatabase<syd::StandardDatabase>();
  output->pixel_unit = syd::FindOrCreatePixelUnit(db, "bool", "Boolean 0|1");
  auto t = syd::FindOrCreateTag(db, success->GetTagName());
  syd::AddTag(output->tags, t);
  db->Update(output);
  return output;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::vector syd::TimeIntegratedActivityImageBuilder::
InsertDebugOutputImages(std::vector<std::string> & names)
{
  syd::Image::vector outputs;
  auto img = images_[0];
  auto db = img->GetDatabase<syd::StandardDatabase>();

  for(auto o:all_outputs_) {
    // do nothing for auc or integrate or success output
    if (o->GetTagName() == auc->GetTagName() or
        o->GetTagName() == success->GetTagName() or
        o->GetTagName() == integrate->GetTagName()) continue;

    syd::Image::pointer output;
    if (o->GetTagName() != params->GetTagName()) {
      output = syd::InsertImage<ImageType>(o->GetImage(), img->patient);
    }
    if (o->GetTagName() == params->GetTagName()) {
      auto oo = std::dynamic_pointer_cast<syd::FitOutputImage_ModelParams>(o);
      typedef FitOutputImage_ModelParams::Image4DType Image4DType;
      output = syd::InsertImage<Image4DType>(oo->GetImage4D(), img->patient);
    }
    syd::SetImageInfoFromImage(output, img);
    output->tags.clear(); // remove all tags for the debug images
    auto t = syd::FindOrCreateTag(db, o->GetTagName());
    syd::AddTag(output->tags, t);
    output->pixel_unit = syd::FindPixelUnit(db, "no_unit");
    db->Update(output);
    outputs.push_back(output);
    names.push_back(o->GetTagName());
  }
  return outputs;
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
syd::FitImages::pointer syd::TimeIntegratedActivityImageBuilder::
Run()
{
  // Check input data and get times
  auto times = CheckInputs();

  // Get itk image
  std::vector<ImageType::Pointer> itk_images;
  for(auto input:images_)
    itk_images.push_back(syd::ReadImage<ImageType>(input->GetAbsolutePath()));

  // Build mask
  auto mask = CreateMaskFromThreshold(itk_images, min_activity_);
  auto additional_mask = FindAdditionalMask();
  if (additional_mask != nullptr) {
    if (!syd::ImagesHaveSameSupport<MaskImageType,MaskImageType>(mask, additional_mask))
      additional_mask = syd::ResampleAndCropImageLike<MaskImageType>(additional_mask, mask, 0, 0);
    mask = syd::AndImage<MaskImageType>(mask, additional_mask);
  }
  int nb_pixels = syd::ComputeSumOfPixelValues<MaskImageType>(mask);

  // set filter info
  filter_.ClearInput();
  for(auto i=0; i<times.size(); i++)
    filter_.AddInput(itk_images[i], times[i]);
  if (options_.GetLambdaDecayConstantInHours() == 0.0) {
    options_.SetLambdaDecayConstantInHours(images_[0]->injection->GetLambdaDecayConstantInHours());
  }
  filter_.SetMask(mask);
  if (options_.GetRestrictedFlag()) filter_.AddOutputImage(auc);
  else filter_.AddOutputImage(integrate);
  filter_.AddOutputImage(success);
  if (debug_images_flag_) {
    if (options_.GetRestrictedFlag()) filter_.AddOutputImage(integrate);
    else filter_.AddOutputImage(auc);
    filter_.AddOutputImage(r2);
    filter_.AddOutputImage(best_model);
    filter_.AddOutputImage(iter);
    filter_.AddOutputImage(params);
    filter_.AddOutputImage(mrt);
  }
  filter_.SetOptions(options_);

  // Print
  std::string sm;
  auto models = options_.GetModels();
  for(auto m:models) sm += m->GetName()+"("+std::to_string(m->GetId())+") ";

  LOG(1) << "Starting fit: "
         << "t= " << syd::ArrayToString(filter_.GetTimes(), 2)
         << " ; m= " << sm << "; "
         << (min_activity_>0.0 ? "with mask_min_activity":"no_mask_min_activity")
         << " ; pixels= " << nb_pixels
         << " ; R2_min= " << options_.GetR2MinThreshold() << " ; "
         << (options_.GetRestrictedFlag() ? "restricted":"non restricted")
         << " ; " << options_.GetAkaikeCriterion();

  // Go !
  filter_.Run();

  // Create output (FitImages)
  auto db = images_[0]->GetDatabase();
  auto tia = db->New<syd::FitImages>();
  for(auto in:images_) tia->images.push_back(in);
  tia->min_activity = min_activity_;
  tia->SetFromOptions(options_);
  tia->nb_pixels = GetFilter().GetNumberOfPixels();
  tia->nb_success_pixels = GetFilter().GetNumberOfSuccessfullyFitPixels();

  // Outputs
  auto output = InsertOutputAUCImage();
  auto s = InsertOutputSuccessFitImage(mask);
  tia->AddOutput(output, "fit_auc");
  tia->AddOutput(s, success->GetTagName());
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
  // tia->comments.push_back("tia builder "+Now());

  return tia;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::vector<double> syd::TimeIntegratedActivityImageBuilder::
CheckInputs()
{
  // for model >= f1, Check nb of images
  if (options_.GetModelsName() != "f0" && images_.size() < 2) {
    EXCEPTION("Error at least 2 images needed");
  }

  // Check injection
  syd::Injection::pointer injection = images_[0]->injection;
  bool b = true;
  for(auto image:images_) {
    if (image->injection == nullptr) {
      EXCEPTION("Images need to be associated with injection.");
    }
    b = b and (injection->id == image->injection->id);
  }
  if (!b) {
    EXCEPTION("The image do not have the same injection.");
  }

  // Sort images
  auto db = images_[0]->GetDatabase();
  db->Sort(images_, "acquisition_date");

  // Get times
  auto times = syd::GetTimesFromInjection(images_);

  // Check times
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


// --------------------------------------------------------------------
typename syd::TimeIntegratedActivityImageBuilder::MaskImageType::Pointer
syd::TimeIntegratedActivityImageBuilder::FindAdditionalMask()
{
  if (additional_mask_name_ != "none") {
    auto mask = syd::FindOneRoiMaskImage(images_[0], additional_mask_name_);
    if (mask == nullptr) {
      EXCEPTION("Cannot find mask " << additional_mask_name_
                << " for this image " << images_[0]);
    }
    return syd::ReadImage<MaskImageType>(mask->GetAbsolutePath());
  }
  return nullptr;
}
// --------------------------------------------------------------------
