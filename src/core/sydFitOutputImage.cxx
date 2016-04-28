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
#include "sydTimeIntegratedActivityFilter.h"
#include "sydImageUtils.h"

// --------------------------------------------------------------------
syd::FitOutputImage::FitOutputImage()
{
  UseImageFlag = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage::InitImage(Pointer input)
{
  image = syd::CreateImageLike<ImageType>(input);
  iterator = Iterator(image, image->GetLargestPossibleRegion());
  image->FillBuffer(0.0);
  UseImageFlag = true;
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
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_AUC::Update()
{
  double r = model_->ComputeAUC(initial_tac_, index_);
  SetValue(r);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_Integrate::FitOutputImage_Integrate():FitOutputImage()
{
  filename = "integrate.mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_Integrate::Update()
{
  double r = model_->Integrate();
  SetValue(r);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_R2::FitOutputImage_R2():FitOutputImage()
{
  filename = "r2.mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_R2::Update()
{
  double R2 = model_->ComputeR2(working_tac_);
  SetValue(R2);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_Model::FitOutputImage_Model():FitOutputImage()
{
  filename = "best_model.mhd";
  SetValue(0);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_Model::Update()
{
  int id = model_->id_;
  SetValue(id);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_Iteration::FitOutputImage_Iteration():FitOutputImage()
{
  filename = "iteration.mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_Iteration::Update()
{
  int it = model_->ceres_summary_.num_unsuccessful_steps +
    model_->ceres_summary_.num_successful_steps;
  SetValue(it);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_Success::FitOutputImage_Success():FitOutputImage()
{
  filename = "success.mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_Success::Update()
{
  SetValue(1);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_EffHalfLife::FitOutputImage_EffHalfLife():FitOutputImage()
{
  filename = "ehl.mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_EffHalfLife::Update()
{
  double h = model_->GetEffHalfLife();
  SetValue(h);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_Lambda::FitOutputImage_Lambda():FitOutputImage()
{
  filename = "l1.mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_Lambda::Update()
{
  double h = model_->GetLambda(0);
  SetValue(h);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_NbOfPointsForFit::FitOutputImage_NbOfPointsForFit():FitOutputImage()
{
  filename = "nbfit.mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_NbOfPointsForFit::Update()
{
  SetValue(working_tac_->size());
}
// --------------------------------------------------------------------
