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

#ifndef SYDTIMEINTEGRATEDACTIVITYIMAGEBUILDERBASE_H
#define SYDTIMEINTEGRATEDACTIVITYIMAGEBUILDERBASE_H

// syd
#include "sydDatabaseFilter.h"
#include "sydTimeIntegratedActivityFilter.h"
#include "sydImageBuilder.h"

// --------------------------------------------------------------------
namespace syd {

  /// This base class is used to create TimeIntegratedActivityImageBuilder class that perform operation on Images
  /// series and images.
  class TimeIntegratedActivityImageBuilder:
    public syd::ImageBuilder,
    public syd::TimeIntegratedActivityFilter {

  public:
    /// Constructor.
    TimeIntegratedActivityImageBuilder(syd::StandardDatabase * db);

    /// Set input images
    void SetInput(const syd::Image::vector images);
    void SetPreProcessingGaussianFilter(double g);
    void SetMinimumValueMask(const double min_activity);
    void SetDebugImagesFlag(bool debug);
    void SetPostProcessingMedianFilter(bool median);
    void SetPostProcessingFillHoles(int radius);
    void SetTimes(std::vector<double> times) { times_ = times; use_time_from_image_flag_ = false; }
    void UseTimesFromImage(bool b) { use_time_from_image_flag_ = b; }

    // Helper
    std::string PrintOptions() const;

    // Main functions
    void RunPreProcessing(std::vector<ImageType::Pointer> & images);
    void CreateTimeIntegratedActivityImage();
    void RunPostProcessing();
    void InsertOutputImagesInDB(std::vector<std::string> & tag_names);

    // Output
    syd::Image::pointer GetTimeIntegratedActivityImage() { return tia_; }

  protected:
    syd::Image::vector inputs_;
    bool use_time_from_image_flag_;
    double min_activity_;
    bool debug_images_flag_;
    bool median_flag_;
    int fill_holes_radius_;
    double gauss_;
    int nb_pixels_;

    syd::Image::pointer tia_;

  }; // class TimeIntegratedActivityImageBuilder

} // namespace syd
// --------------------------------------------------------------------

#endif
