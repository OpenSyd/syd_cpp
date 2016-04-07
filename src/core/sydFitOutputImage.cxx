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
#include "sydIntegratedActivityImageBuilder.h"
#include "sydImageUtils.h"

// --------------------------------------------------------------------
syd::FitOutputImage::FitOutputImage()
{
  use_current_tac = false;
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
syd::FitOutputImage_AUC::FitOutputImage_AUC(double l):FitOutputImage()
{
  filename = "auc.mhd";
  lambda_phys_hours_ = l;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_AUC::Update(const syd::TimeActivityCurve & tac,
                                     const syd::TimeActivityCurve & restricted_tac,
                                     const syd::FitModelBase * model)
{
  DD("Update auc");
  double r = model->ComputeAUC(tac, lambda_phys_hours_, use_current_tac);
  DD(r);
  DD(tac);
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
void syd::FitOutputImage_R2::Update(const syd::TimeActivityCurve & tac,
                                    const syd::TimeActivityCurve & restricted_tac,
                                    const syd::FitModelBase * model)
{
  double R2 = model->ComputeR2(tac, use_current_tac);
  SetValue(R2);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitOutputImage_Model::FitOutputImage_Model():FitOutputImage()
{
  filename = "best_model.mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitOutputImage_Model::Update(const syd::TimeActivityCurve & tac,
                                       const syd::TimeActivityCurve & restricted_tac,
                                       const syd::FitModelBase * model)
{
  int id = model->id_;
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
void syd::FitOutputImage_Iteration::Update(const syd::TimeActivityCurve & tac,
                                           const syd::TimeActivityCurve & restricted_tac,
                                           const syd::FitModelBase * model)
{
  int it = model->ceres_summary_.num_unsuccessful_steps +
    model->ceres_summary_.num_successful_steps;
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
void syd::FitOutputImage_Success::Update(const syd::TimeActivityCurve & tac,
                                       const syd::TimeActivityCurve & restricted_tac,
                                       const syd::FitModelBase * model)
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
void syd::FitOutputImage_EffHalfLife::Update(const syd::TimeActivityCurve & tac,
                                             const syd::TimeActivityCurve & restricted_tac,
                                             const syd::FitModelBase * model)
{
  double h = model->GetEffHalfLife();
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
void syd::FitOutputImage_Lambda::Update(const syd::TimeActivityCurve & tac,
                                        const syd::TimeActivityCurve & restricted_tac,
                                        const syd::FitModelBase * model)
{
  double h = model->GetLambda(0);
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
void syd::FitOutputImage_NbOfPointsForFit::Update(const syd::TimeActivityCurve & tac,
                                                  const syd::TimeActivityCurve & restricted_tac,
                                                  const syd::FitModelBase * model)
{
  if (restricted_tac_flag_) SetValue(restricted_tac.size());
  else SetValue(tac.size());
}
// --------------------------------------------------------------------
