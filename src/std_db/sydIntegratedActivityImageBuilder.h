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
    //    ~DebugType() { DD(" dest"); }
    int index;
    int x;
    int y;
    int z;
    syd::TimeActivityCurve tac;
    std::vector<syd::FitModelBase*> models;
    std::string name;
    std::vector<ceres::Solver::Summary> summaries;
  };

  /// This class is used to create a pixel-based integrated activity.
  class IntegratedActivityImageBuilder {

  public:
    /// Constructor.
    IntegratedActivityImageBuilder(syd::StandardDatabase * db);

    /// Destructor (empty)
    ~IntegratedActivityImageBuilder() {}

    void SetInput(syd::Image::vector & images);
    syd::Image::pointer GetOutput() const;
    void CreateIntegratedActivityImage();
    double Integrate();

    void InitSolver();

    syd::TimeActivityCurve & GetTAC() { return current_tac_; }

    void SaveDebugPixel(const std::string & filename) const;
    void SaveDebugModel(const std::string & filename) const;
    void AddDebugPixel(std::string name, int x, int y, int z);

  protected:
    syd::StandardDatabase * db_;

    syd::Image::vector images_;
    syd::Image::pointer output_;
    syd::TimeActivityCurve current_tac_;

    // std::vector<int> debug_pixels;
    // std::vector<syd::TimeActivityCurve> debug_tac;

    std::vector<syd::FitModelBase*> models_;
    //    std::vector<syd::FitModelBase*> debug_models_;
    bool current_debug_flag_;

    std::vector<DebugType> debug_data;
    std::vector<DebugType>::iterator debug_current;

    ceres::Solver::Options * ceres_options_;
    ceres::Solver::Summary ceres_summary_;

    //    ceres::Problem problem;

  }; // class IntegratedActivityImageBuilder

#include "sydIntegratedActivityImageBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
