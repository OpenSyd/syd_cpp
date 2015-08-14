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

#ifndef SYDFITOUTPUTIMAGE_H
#define SYDFITOUTPUTIMAGE_H

#include "sydIntegratedActivityImageBuilder.h"
#include "sydImageUtils.h"

namespace syd {

  /// This is the base class for object that store an image as an
  /// output of a pixel based fit process.
  class FitOutputImage {
  public:

    // Image is always, float
    typedef float PixelType;
    typedef itk::Image<PixelType,3> ImageType;
    typedef ImageType::Pointer Pointer;
    typedef itk::ImageRegionIterator<ImageType> Iterator;

    Pointer image;
    Iterator iterator;
    std::string filename;
    bool use_current_tac;

    FitOutputImage(Pointer input) {
      image = syd::CreateImageLike<ImageType>(input);
      iterator = Iterator(image, image->GetLargestPossibleRegion());
      image->FillBuffer(0.0);
      use_current_tac = false;
    }

    virtual void Update(const syd::TimeActivityCurve & tac,
                        const syd::FitModelBase * model) = 0;

  };


  /// Compute and store the AUC (Area Under the Curve)
  class FitOutputImage_AUC: public FitOutputImage {
  public:
    FitOutputImage_AUC(Pointer input):FitOutputImage(input) { filename = "auc.mhd"; }
    virtual void Update(const syd::TimeActivityCurve & tac,
                        const syd::FitModelBase * model) {
      double r = model->ComputeAUC(tac, use_current_tac);
      iterator.Set(r);
    }
  };

  /// Compute and store the coefficient of determination
  class FitOutputImage_R2: public FitOutputImage {
  public:
    FitOutputImage_R2(Pointer input):FitOutputImage(input) { filename = "r2.mhd"; }
    virtual void Update(const syd::TimeActivityCurve & tac,
                        const syd::FitModelBase * model) {
      double R2 = model->ComputeR2(tac, use_current_tac);
      iterator.Set(R2);
    }
  };

  /// Compute and store the id of the selected model
  class FitOutputImage_Model: public FitOutputImage {
  public:
    FitOutputImage_Model(Pointer input):FitOutputImage(input) { filename = "best_model.mhd"; }
    virtual void Update(const syd::TimeActivityCurve & tac,
                        const syd::FitModelBase * model) {
      int id = model->id_;
      iterator.Set(id);
    }
  };

  /// Compute and store the number of iterations needed to converge
  class FitOutputImage_Iteration: public FitOutputImage {
  public:
    FitOutputImage_Iteration(Pointer input):FitOutputImage(input) { filename = "iteration.mhd"; }
    virtual void Update(const syd::TimeActivityCurve & tac,
                        const syd::FitModelBase * model) {
      int it = model->ceres_summary_.num_unsuccessful_steps +
        model->ceres_summary_.num_successful_steps;
      iterator.Set(it);
    }
  };

  /// Store a binary image, pixel is '1' if the fit process is a success
  class FitOutputImage_Success: public FitOutputImage {
  public:
    FitOutputImage_Success(Pointer input):FitOutputImage(input) { filename = "success.mhd"; }
    virtual void Update(const syd::TimeActivityCurve & tac,
                        const syd::FitModelBase * model) {
      iterator.Set(1);
    }
  };

} // end namespace
#endif
