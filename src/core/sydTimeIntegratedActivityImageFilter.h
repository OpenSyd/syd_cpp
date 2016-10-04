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

#ifndef SYDTIMEINTEGRATEDACTIVITYIMAGEFILTER_H
#define SYDTIMEINTEGRATEDACTIVITYIMAGEFILTER_H

// syd
#include "sydImageUtils.h"
#include "sydFitModels.h"
#include "sydFitOutputImage.h"
#include "sydTimeIntegratedActivityFitOptions.h"

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a pixel-based integrated activity.
  class TimeIntegratedActivityImageFilter {

  public:
    /// Constructor.
    TimeIntegratedActivityImageFilter();

    /// Destructor (empty)
    ~TimeIntegratedActivityImageFilter();

    /// Image types
    typedef float PixelType;
    typedef itk::Image<PixelType,3> ImageType;
    typedef itk::Image<PixelType,4> Image4DType;
    typedef itk::ImageRegionIterator<ImageType> Iterator;
    typedef itk::ImageRegionIterator<Image4DType> Iterator4D;
    typedef unsigned char MaskPixelType;
    typedef itk::Image<MaskPixelType,3> MaskImageType;
    typedef itk::ImageRegionIterator<MaskImageType> MaskIterator;

    /// Input
    void AddInput(ImageType::Pointer image, double time);
    void ClearInput() { images_.clear(); }
    void SetMask(MaskImageType::Pointer m) { mask_ = m; }
    void SetLambdaDecayConstantInHours(double l) { lambda_in_hours_ = l; }
    void AddTimePointValue(double time, double value);
    void SetOptions(syd::TimeIntegratedActivityFitOptions & options) { options_ = options; }
    void AddOutputImage(syd::FitOutputImage::pointer o);
    void InitModels();

    /// Main function
    void Run();

    /// Helpers function
    Iterator4D GetIteratorAtPoint(double x, double y, double z);
    syd::TimeActivityCurve::pointer GetCurrentTAC() const { return initial_tac_; }
    syd::TimeActivityCurve::pointer GetWorkingTAC() const { return working_tac_; }
    int GetCurrentIndexRestrictedTAC() const { return current_index_restricted_tac_; }
    syd::FitModelBase::vector GetModels() const { return models_; }
    syd::FitOutputImage::vector GetOutputs() const { return outputs_; }

    /// Do the computation for the current pixel
    int FitOnePixel(Iterator4D it);

  protected:
    /// Input
    std::vector<ImageType::Pointer> images_;
    std::vector<double> times_;
    MaskImageType::Pointer mask_;
    syd::TimeIntegratedActivityFitOptions options_;
    double lambda_in_hours_;
    std::vector<double> additional_point_times;
    std::vector<double> additional_point_values;
    syd::FitModelBase::vector models_;

    /// Computed 4D images that merge all 3D images
    Image4DType::Pointer tac_image_;

    /// Output
    syd::FitOutputImage::vector outputs_;

    /// Options for the solver
    ceres::Solver::Options * ceres_options_;
    ceres::Solver::Summary ceres_summary_;
    ceres::Solver::Summary current_ceres_summary_;

    /// for computation
    syd::TimeActivityCurve::pointer initial_tac_;
    syd::TimeActivityCurve::pointer working_tac_;
    int current_index_restricted_tac_;

    /// Initialize the solver
    void InitSolver();

    /// Initialize the outpus
    void InitOutputs();

    /// Initialize the 4D input
    void Init4DInput();

    /// Initialize the mask
    void InitMask();

    /// Check the inputs (size etc)
    void CheckInputs();

    /// Try to fit a TAC with the given model
    void FitTACWithModel(syd::FitModelBase::pointer model,
                         syd::TimeActivityCurve::pointer tac);

    /// Choose the best lmode according to Akaike criterion
    int SelectBestModel(syd::FitModelBase::vector models,
                        syd::TimeActivityCurve::pointer tac);

    /// Compute the restricted tac from the max (3 points at min)
    int GetRestrictedTac(syd::TimeActivityCurve::pointer initial_tac,
                         syd::TimeActivityCurve::pointer restricted_tac);


  }; // class TimeIntegratedActivityImageFilter

} // namespace syd
// --------------------------------------------------------------------

#endif
