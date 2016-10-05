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

#ifndef SYDTIMEINTEGRATEDACTIVITYIMAGEBUILDER_H
#define SYDTIMEINTEGRATEDACTIVITYIMAGEBUILDER_H

// syd
#include "sydImage.h"
#include "sydStandardDatabase.h"
#include "sydTimeIntegratedActivityImageFilter.h"

// --------------------------------------------------------------------
namespace syd {

  /// This base class is used to create time integrated image
  /// from an image series
  class TimeIntegratedActivityImageBuilder {

  public:
    /// Constructor.
    TimeIntegratedActivityImageBuilder();

    /// Set input images
    void SetInput(const syd::Image::vector images);

    /// Set image threshold for initial mask
    void SetImageActivityThreshold(const double min_activity);

    /// Debug intermediate images
    void SetDebugOutputFlag(bool debug);

    /// Set the fit options
    void SetOptions(syd::TimeIntegratedActivityFitOptions options);

    /// Main functions
    void Run();

    /// Output: get the main output and insert in the db
    syd::Image::pointer InsertOutputImage();

    /// Output: write mhd debug
    void WriteDebugOutput();

    /// Output: insert the debug images in the db
    syd::Image::vector InsertDebugOutputImages();

    /// Return the main filter
    syd::TimeIntegratedActivityImageFilter & GetFilter() { return filter_; }

  protected:
    syd::Image::vector inputs_;
    double min_activity_;
    bool debug_images_flag_;

    // list of outputs
    syd::FitOutputImage_AUC::pointer auc;
    syd::FitOutputImage_Integrate::pointer integrate;
    syd::FitOutputImage_R2::pointer r2;
    syd::FitOutputImage_Model::pointer best_model;
    syd::FitOutputImage_Iteration::pointer iter;
    syd::FitOutputImage_Success::pointer success;

    /// Image types
    typedef syd::TimeIntegratedActivityImageFilter::ImageType ImageType;
    typedef syd::TimeIntegratedActivityImageFilter::MaskImageType MaskImageType;
    typedef syd::TimeIntegratedActivityImageFilter::Iterator Iterator;
    typedef syd::TimeIntegratedActivityImageFilter::MaskIterator MaskIterator;

    syd::TimeIntegratedActivityImageFilter filter_;
    syd::TimeIntegratedActivityFitOptions options_;
    syd::FitOutputImage::vector all_outputs_;

    std::vector<double> CheckInputs();
    typename MaskImageType::Pointer
    CreateMaskFromThreshold(std::vector<ImageType::Pointer> itk_images,
                            double min_activity);

  }; // class TimeIntegratedActivityImageBuilder

} // namespace syd
// --------------------------------------------------------------------


#endif
