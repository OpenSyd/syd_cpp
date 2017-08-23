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
#include "sydFitOutputImage.h"
#include "sydTimeIntegratedActivityFilter.h"

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a pixel-based integrated activity.
  class TimeIntegratedActivityImageFilter:
    public TimeIntegratedActivityFilter
  {

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
    void AddOutputImage(syd::FitOutputImage::pointer o);

    /// Main function
    void Run();

    /// Helpers function
    Iterator4D GetIteratorAtPoint(double x, double y, double z);
    syd::FitOutputImage::vector GetOutputs() const { return outputs_; }
    std::vector<double> GetTimes() const { return times_; }
    int GetNumberOfPixels() const { return nb_pixels_; }
    int GetNumberOfSuccessfullyFitPixels() const { return nb_successful_fit_; }

    /// Do the computation for the current pixel
    int FitOnePixel(Iterator4D it);

  protected:
    /// Input
    std::vector<ImageType::Pointer> images_;
    std::vector<double> times_;
    MaskImageType::Pointer mask_;
    int nb_pixels_;
    int nb_successful_fit_;

    /// Computed 4D images that merge all 3D images
    Image4DType::Pointer tac_image_;

    /// Output
    syd::FitOutputImage::vector outputs_;

    /// Initialize the outpus
    void InitOutputs();

    /// Initialize the 4D input
    void Init4DInput();

    /// Initialize the mask
    void InitMask();

    /// Check the inputs (size etc)
    virtual void CheckInputs();

  }; // class TimeIntegratedActivityImageFilter

} // namespace syd
// --------------------------------------------------------------------

#endif
