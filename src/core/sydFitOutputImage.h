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

  // --------------------------------------------------------------------
  /// This is the base class for object that store an image as an
  /// output of a pixel based fit process.
  class FitOutputImage {
  public:

    // Image is always, float
    typedef float PixelType;
    typedef itk::Image<PixelType,3> ImageType;
    typedef ImageType::Pointer Pointer;
    typedef itk::ImageRegionIterator<ImageType> Iterator;

    FitOutputImage();
    void InitImage(Pointer input);
    void SetValue(double v);
    virtual void Update() = 0;

    Pointer image;
    Iterator iterator;
    std::string filename;
    double value;
    bool UseImageFlag;
    syd::TimeActivityCurve::pointer initial_tac_;
    syd::TimeActivityCurve::pointer working_tac_;
    syd::FitModelBase * model_;
  };


  // --------------------------------------------------------------------
  /// Compute and store the AUC (Area Under the Curve)
  class FitOutputImage_AUC: public FitOutputImage {
  public:
    int index_;
    FitOutputImage_AUC();
    virtual void Update();
  };

  /// Compute and store the Integrate (Area Under the Curve)
  class FitOutputImage_Integrate: public FitOutputImage {
  public:
    FitOutputImage_Integrate();
    virtual void Update();
  };

  /// Compute and store the coefficient of determination
  class FitOutputImage_R2: public FitOutputImage {
  public:
    FitOutputImage_R2();
    virtual void Update();
  };

  /// Compute and store the id of the selected model
  class FitOutputImage_Model: public FitOutputImage {
  public:
    FitOutputImage_Model();
    virtual void Update();
  };

  /// Compute and store the number of iterations needed to converge
  class FitOutputImage_Iteration: public FitOutputImage {
  public:
    FitOutputImage_Iteration();
    virtual void Update();
  };

  /// Store a binary image, pixel is '1' if the fit process is a success
  class FitOutputImage_Success: public FitOutputImage {
  public:
    FitOutputImage_Success();
    virtual void Update();
  };

  /// Store image with effective half life (l1+l_phys, first expo only)
  class FitOutputImage_EffHalfLife: public FitOutputImage {
  public:
    FitOutputImage_EffHalfLife();
    virtual void Update();
  };

  /// Store image with lambda
  class FitOutputImage_Lambda: public FitOutputImage {
  public:
    FitOutputImage_Lambda();
    virtual void Update();
  };

  /// Store image with nb of points used for fit
  class FitOutputImage_NbOfPointsForFit: public FitOutputImage {
  public:
    FitOutputImage_NbOfPointsForFit();
    virtual void Update();
  };

} // end namespace

#endif
