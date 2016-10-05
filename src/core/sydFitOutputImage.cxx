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

#include "sydFitOutputImage.h"
#include "sydImageUtils.h"

// --------------------------------------------------------------------
syd::FitOutputImage::FitOutputImage()
{
  UseImageFlag = false;
  tag = "";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage::InitImageLike(Pointer input)
{
  image = syd::CreateImageLike<ImageType>(input);
  iterator = Iterator(image, image->GetLargestPossibleRegion());
  image->FillBuffer(0.0);
  UseImageFlag = true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage::SetInitialTimeActivityCurve(syd::TimeActivityCurve::pointer tac)
{
  initial_tac_ = tac;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage::SetWorkingTimeActivityCurve(syd::TimeActivityCurve::pointer tac)
{
  working_tac_ = tac;
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::FitOutputImage::Iterate()
{
  ++iterator;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage::WriteImage()
{
  syd::WriteImage<ImageType>(image, filename);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage::SetValue(double v)
{
  if (UseImageFlag) iterator.Set(v);
  else value = v;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_AUC::FitOutputImage_AUC():FitOutputImage()
{
  filename = "auc.mhd";
  tag = "fit_auc";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_AUC::Update(syd::FitModelBase::pointer model)
{
  auto index = initial_tac_->size() - working_tac_->size();
  double r = model->ComputeAUC(initial_tac_, index);
  SetValue(r);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_Integrate::FitOutputImage_Integrate():FitOutputImage()
{
  filename = "integrate.mhd";
  tag = "fit_integrate";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_Integrate::Update(syd::FitModelBase::pointer model)
{
  double r = model->Integrate();
  SetValue(r);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_R2::FitOutputImage_R2():FitOutputImage()
{
  filename = "r2.mhd";
  tag = "fit_r2";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_R2::Update(syd::FitModelBase::pointer model)
{
  double R2 = model->ComputeR2(working_tac_);
  SetValue(R2);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_Model::FitOutputImage_Model():FitOutputImage()
{
  filename = "best_model.mhd";
  SetValue(0);
  tag = "fit_best_model";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_Model::Update(syd::FitModelBase::pointer model)
{
  int id = model->GetId();
  SetValue(id);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_Iteration::FitOutputImage_Iteration():FitOutputImage()
{
  filename = "iteration.mhd";
  tag = "fit_nb_iterations";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_Iteration::Update(syd::FitModelBase::pointer model)
{
  auto summary = model->GetSummary();
  int it = summary.num_unsuccessful_steps +
    summary.num_successful_steps;
  SetValue(it);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_Success::FitOutputImage_Success():FitOutputImage()
{
  filename = "success.mhd";
  tag = "fit_success";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_Success::Update(syd::FitModelBase::pointer model)
{
  SetValue(1);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_EffHalfLife::FitOutputImage_EffHalfLife():FitOutputImage()
{
  filename = "ehl.mhd";
  tag = "fit_ehl";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_EffHalfLife::Update(syd::FitModelBase::pointer model)
{
  double h = model->GetEffHalfLife();
  SetValue(h);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_Lambda::FitOutputImage_Lambda():FitOutputImage()
{
  filename = "l1.mhd";
  tag = "fit_l1";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_Lambda::Update(syd::FitModelBase::pointer model)
{
  double h = model->GetLambda(0);
  SetValue(h);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_NbOfPointsForFit::FitOutputImage_NbOfPointsForFit():FitOutputImage()
{
  filename = "nbfit.mhd";
  tag = "fit_nb_fit";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_NbOfPointsForFit::Update(syd::FitModelBase::pointer model)
{
  SetValue(working_tac_->size());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_ModelParams::FitOutputImage_ModelParams():FitOutputImage()
{
  filename = "params.mhd";
  tag = "fit_params";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_ModelParams::InitImage(Pointer input)
{
  image_4d = Image4DType::New();
  Image4DType::RegionType region;
  Image4DType::SizeType size;
  Image4DType::IndexType start;
  Image4DType::SpacingType spacing;
  Image4DType::PointType origin;
  for(auto i=0; i<3; i++) {
    size[i] = input->GetLargestPossibleRegion().GetSize()[i];
    start[i] = input->GetLargestPossibleRegion().GetIndex()[i];
    spacing[i] = input->GetSpacing()[i];
    origin[i] = input->GetOrigin()[i];
  }
  size[3] = 4; // maximum nb of params
  start[3] = 0;
  spacing[3] = 1.0;
  origin[3] = 0.0;
  region.SetSize(size);
  region.SetIndex(start);
  image_4d->SetRegions(region);
  image_4d->SetSpacing(spacing);
  image_4d->SetOrigin(origin);
  image_4d->Allocate();
  image_4d->FillBuffer(0.0);
  raw_pointer = image_4d->GetBufferPointer();
  offset = size[0]*size[1]*size[2];
  UseImageFlag = true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_ModelParams::Iterate()
{
  ++raw_pointer;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_ModelParams::Update(syd::FitModelBase::pointer model)
{
  if (UseImageFlag) {
    auto p = model->GetParameters();
    auto iter = raw_pointer;
    for(auto i=0; i<p.size(); i++) {
      *iter = p[i];
      iter += offset;
    }
    for(auto i=p.size(); i<4; i++) {
      *iter = 0.0;
      iter += offset;
    }
  }
  else {
    value = model->GetParameters()[0];
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_ModelParams::WriteImage()
{
  syd::WriteImage<Image4DType>(image_4d, filename);
}
// --------------------------------------------------------------------
