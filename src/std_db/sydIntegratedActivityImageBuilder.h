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
#include "sydStandardDatabase.h"
#include "sydImage.h"
#include "sydFitModels.h"

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
    std::vector<ceres::Solver::Summary> summaries; // FIXME to put in model
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
    IntegratedActivityImageBuilder(syd::StandardDatabase * db);

    /// Destructor (empty)
    ~IntegratedActivityImageBuilder() {}

    typedef float PixelType;
    typedef itk::Image<PixelType,3> ImageType;
    typedef itk::Image<PixelType,4> Image4DType;

    void SetInput(syd::Image::vector & images);
    syd::Image::pointer GetOutput() const; // FIXME to change

    // Main function
    void CreateIntegratedActivityImage();

    // protected
    void InitSolver();
    void FitModels(TimeActivityCurve & tac, bool debug_this_point_flag, DebugType * debug_current);
    void ReadAndInitInputData(std::vector<ImageType::Pointer> & itk_images,
                              Image4DType::Pointer tac_image);

    // Debug
    void SaveDebugPixel(const std::string & filename) const;
    void SaveDebugModel(const std::string & filename) const;
    void AddDebugPixel(std::string name, int x, int y, int z);
    bool debug_only_flag_;

    // options
    double robust_scaling_;
    bool gauss_sigma_;
    double activity_threshold_;

  protected:
    syd::StandardDatabase * db_;

    syd::Image::vector images_;
    syd::Image::pointer output_; // FIXME to remove ?

    // List of all tested models
    std::vector<syd::FitModelBase*> models_;

    // Current selected models
    syd::FitModelBase * current_model_;

    // Options for the solver
    ceres::Solver::Options * ceres_options_;
    ceres::Solver::Summary ceres_summary_;
    ceres::Solver::Summary current_ceres_summary_;

    // Debug
    bool current_debug_flag_;
    std::vector<DebugType> debug_data;
    std::vector<DebugType>::iterator debug_current;

  }; // class IntegratedActivityImageBuilder

#include "sydIntegratedActivityImageBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
