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

#ifndef SYDTIMEINTEGRATEDACTIVITYFILTER_H
#define SYDTIMEINTEGRATEDACTIVITYFILTER_H

// syd
#include "sydImageUtils.h"
#include "sydFitModels.h"
#include "sydFitOutputImage.h"

// --------------------------------------------------------------------
namespace syd {

  class FitOutputImage;
  class FitOutputImage_Success;
  class FitOutputImage_AUC;
  class FitOutputImage_Integrate;

  class ModelResult {
  public:
    syd::FitModelBase * model;
    syd::TimeActivityCurve * tac;
    syd::TimeActivityCurve * restricted_tac;
    double R2;
    double AICc;
  };

  /// This class is used to create a pixel-based integrated activity.
  class TimeIntegratedActivityFilter {

  public:
    /// Constructor.
    TimeIntegratedActivityFilter();

    /// Destructor (empty)
    ~TimeIntegratedActivityFilter() {}

    typedef float PixelType;
    typedef itk::Image<PixelType,3> ImageType;
    typedef itk::Image<PixelType,4> Image4DType;
    typedef itk::ImageRegionIterator<ImageType> Iterator3D;
    typedef itk::ImageRegionIterator<Image4DType> Iterator4D;

    typedef unsigned char MaskPixelType;
    typedef itk::Image<MaskPixelType,3> MaskImageType;

    // Input
    void SetR2MinThreshold(double r) { R2_min_threshold_ = r; }
    void SetRestrictedTACFlag(bool b) { restricted_tac_flag_ = b; }
    void SetModels(const std::vector<std::string> & model_names);
    void SetModel(const std::string & model_name);

    // Internal inputs
    void AddInput(ImageType::Pointer image, double time) { images_.push_back(image); times_.push_back(time); }
    void SetInputTAC(syd::TimeActivityCurve & tac) { tac_ = tac; }
    void AddModel(syd::FitModelBase * m, int id);
    void AddOutputImage(syd::FitOutputImage * o) { outputs_.push_back(o); }
    void SetMask(ImageType::Pointer m) { mask_ = m; }
    void SetLambdaPhysicHours(double l) { image_lambda_phys_in_hour_ = l; }
    void SetAdditionalPoint(bool b, double time, double value);

    // Helpers
    int GetRestrictedTac(syd::TimeActivityCurve::pointer initial_tac,
                         syd::TimeActivityCurve::pointer restricted_tac);
    bool GetRestrictedTacFlag() const { return restricted_tac_flag_; }

    // Other functions
    void ClearModel() { models_.clear(); }
    std::vector<syd::FitModelBase*> & GetModels() { return models_; }

    // Main function
    void CreateIntegratedActivityImage();
    syd::FitModelBase* CreateIntegratedActivity(syd::TimeActivityCurve::pointer initial_tac);

    std::vector<double> GetTimes() { return times_; }

    // Outputs
    void ClearOutputs() { outputs_.clear(); }
    syd::FitOutputImage_Success * GetSuccessOutput() { return success_output_; }
    syd::FitOutputImage * GetOutput();
    syd::FitOutputImage_AUC * GetAUCOutput() { return auc_output_; }
    std::vector<syd::FitOutputImage*> & GetOutputs() { return outputs_; }


  protected:

    // Input
    std::vector<ImageType::Pointer> images_;
    std::vector<double> times_;
    syd::TimeActivityCurve tac_;

    // Computed 4D images that merge all 3D images
    Image4DType::Pointer tac_image_;

    // List of all tested models
    std::vector<syd::FitModelBase*> models_;
    std::vector<syd::FitModelBase*> all_models_;

    // Current selected models
    syd::FitModelBase * current_model_;

    // Output
    syd::FitOutputImage_Success * success_output_;
    syd::FitOutputImage_AUC * auc_output_;
    syd::FitOutputImage_Integrate * integrate_output_;

    void InitSolver();
    int FitModels(syd::TimeActivityCurve::pointer tac);
    void InitInputData();

    // If 'true', only use the last part of the tac, from the max value to then end (2 points at min)
    bool restricted_tac_flag_;
    double R2_min_threshold_;
    double image_lambda_phys_in_hour_;
    std::vector<syd::FitOutputImage*> outputs_;
    ImageType::Pointer mask_;

    // Options for the solver
    ceres::Solver::Options * ceres_options_;
    ceres::Solver::Summary ceres_summary_;
    ceres::Solver::Summary current_ceres_summary_;

    // When the biological half life is known
    bool additional_point_flag_;
    double additional_point_time_;
    double additional_point_value_;

  }; // class TimeIntegratedActivityFilter

} // namespace syd
// --------------------------------------------------------------------

#endif
