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

#ifndef SYDTIMEINTEGRATEDSPECTIMAGEFILTER_H
#define SYDTIMEINTEGRATEDSPECTIMAGEFILTER_H

// syd
#include "sydTimeActivityCurveIntegrate.h"
#include "sydImage.h"

// --------------------------------------------------------------------
namespace syd {

  class TimeIntegratedSpectImageFilter: public itk::ImageSource<itk::Image<double, 3> > {

  public:

    // Types
    typedef double PixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    typedef ImageType::RegionType RegionType;
    typedef itk::ImageRegionIterator<ImageType> IteratorType;
    typedef TimeIntegratedSpectImageFilter Self;
    typedef itk::ImageSource<ImageType>   Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    itkNewMacro(Self);

    TimeIntegratedSpectImageFilter();
    ~TimeIntegratedSpectImageFilter();

    void SetDebugFlag(bool b) { debug_flag_ = b; }
    void AddInput(double time, ImageType::Pointer spect, double factor);
    void SetMinimumActivityValue(double min_value) { min_activity_value_ = min_value; }
    void SetDefaultPixelValue(double d) { defaultPixelValue_ = d; }
    void SetProgressBarFlag(bool b) { progressBarFlag_ = b; }
    ImageType::Pointer GetOutput() { return output_; }

    void Initialise();
    void Update();

  protected:
    bool isInitialised_;
    bool debug_flag_;
    bool progressBarFlag_;
    std::vector<ImageType::Pointer> spects_;
    std::vector<double> times_;
    std::vector<double> factors_;
    double min_activity_value_;
    ImageType::Pointer output_;
    ImageType::PixelType defaultPixelValue_;

    // debug images
    std::vector<ImageType::Pointer> debug_images;
    std::vector<IteratorType> debug_iterators;
    std::vector<std::string> debug_names;
    int AddDebugImage(std::string name);
    ImageType::Pointer image_lambda;
    ImageType::Pointer image_A;
    ImageType::Pointer image_First;
    ImageType::Pointer image_Middle;
    ImageType::Pointer image_Final;
    ImageType::Pointer image_mask;
    ImageType::Pointer image_nbiteration;
    ImageType::Pointer image_nbpoints;

  }; // end class

} // end namespace
// --------------------------------------------------------------------

#endif