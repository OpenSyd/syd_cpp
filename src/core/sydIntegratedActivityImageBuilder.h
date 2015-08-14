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

#ifndef SYDINTEGRATEDACTIVITYIMAGEBUILDER_H
#define SYDINTEGRATEDACTIVITYIMAGEBUILDER_H

// syd
#include "sydImageUtils.h"
#include "sydFitModels.h"
#include "sydFitOutputImage.h"

// --------------------------------------------------------------------
namespace syd {

  class DebugType {
  public:
    int index;
    int x;
    int y;
    int z;
    syd::TimeActivityCurve tac;
    std::vector<syd::FitModelBase*> models;
    std::string name;
    int selected_model;
  };

  class ModelResult {
  public:
    syd::FitModelBase * model;
    syd::TimeActivityCurve * tac;
    syd::TimeActivityCurve * restricted_tac;
    double R2;
    double AICc;
  };

  /// This class is used to create a pixel-based integrated activity.
  class IntegratedActivityImageBuilder {

  public:
    /// Constructor.
    IntegratedActivityImageBuilder();

    /// Destructor (empty)
    ~IntegratedActivityImageBuilder() {}

    typedef float PixelType;
    typedef itk::Image<PixelType,3> ImageType;
    typedef itk::Image<PixelType,4> Image4DType;
    typedef itk::ImageRegionIterator<ImageType> Iterator3D;
    typedef itk::ImageRegionIterator<Image4DType> Iterator4D;

    // Input
    void AddInput(ImageType::Pointer image, double time) { images_.push_back(image); times_.push_back(time); }
    void AddModel(syd::FitModelBase * m) { models_.push_back(m); }
    void AddOutputImage(syd::FitOutputImage * o) { outputs_.push_back(o); }
    void SetMask(ImageType::Pointer m) { mask_ = m; }

    // Main function
    void CreateIntegratedActivityImage();

    // Other functions
    void ClearModel() { models_.clear(); }

    // Debug
    void SaveDebugPixel(const std::string & filename) const;
    void SaveDebugModel(const std::string & filename) const;
    void AddDebugPixel(std::string name, int x, int y, int z);
    bool debug_only_flag_;

    // options
    //double robust_scaling_;
    double R2_min_threshold_;
    double image_lambda_phys_in_hour_;
    bool restricted_tac_flag_;
    std::vector<FitOutputImage*> outputs_;
    ImageType::Pointer mask_;

    // Debug
    bool current_debug_flag_;
    std::vector<DebugType> debug_data;

  protected:

    // Input
    std::vector<ImageType::Pointer> images_;
    std::vector<double> times_;

    // Computed 4D images that merge all 3D images
    Image4DType::Pointer tac_image_;

    // List of all tested models
    std::vector<syd::FitModelBase*> models_;

    // Current selected models
    syd::FitModelBase * current_model_;

    void InitSolver();
    int FitModels(TimeActivityCurve & tac, bool debug_this_point_flag, DebugType * debug_current);
    void InitInputData();

    // Options for the solver
    ceres::Solver::Options * ceres_options_;
    ceres::Solver::Summary ceres_summary_;
    ceres::Solver::Summary current_ceres_summary_;

  }; // class IntegratedActivityImageBuilder

#include "sydIntegratedActivityImageBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
