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

// syd
#include "sydKmeansInputDataBuilder.h"

// itk
#include <itkRescaleIntensityImageFilter.h>
#include <itkNormalizeImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkAdaptiveHistogramEqualizationImageFilter.h>
#include <itkLogImageFilter.h>
#include <itkImageRegionIterator.h>

// --------------------------------------------------------------------
syd::KmeansInputDataBuilder::KmeansInputDataBuilder()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansInputDataBuilder::SetMask(ImageType::Pointer m)
{
  mask = m;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansInputDataBuilder::AddInput(ImageType::Pointer image)
{
  input_images.push_back(image);
  DD(input_images.size());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansInputDataBuilder::AddInput(Image4DType::Pointer image,
                                           std::vector<int> indices)
{
  input_vector_images.push_back(image);
  // compute the offsets
  int offset = image->GetLargestPossibleRegion().GetSize()[0]*
    image->GetLargestPossibleRegion().GetSize()[1]*
    image->GetLargestPossibleRegion().GetSize()[2];
  for(auto & i:indices) i = i*offset;
  input_vector_images_offsets.push_back(indices);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansInputDataBuilder::BuildInputData()
{
  InsertVectorImagesAsImages();
  PreProcessing();

  //syd::WriteImage<ImageType>(input_images[0], "vector-norm.mhd");

  // FIXME mask exist ?

  // FIXME check images sizes must be the same

  // Declare mask iterator
  IteratorType iter_mask(mask, mask->GetLargestPossibleRegion());

  // Declare image iterator
  std::vector<IteratorType> iter_images;
  for(auto & im:input_images)
    iter_images.push_back(IteratorType(im, im->GetLargestPossibleRegion()));

  // Declare vector iterator
  // std::vector<PixelType*> iter_vector_images;
  // for(auto & im:input_vector_images)
  //   iter_vector_images.push_back(im->GetBufferPointer());

  // Move iterators to begin
  iter_mask.GoToBegin();
  for(auto & a:iter_images) a.GoToBegin();

  // Get nb of pixels
  int nb_pixels = mask->GetLargestPossibleRegion().GetNumberOfPixels();
  int output_offset = nb_pixels;

  // Get nb of dimensions
  nb_dimensions = input_images.size();
  DD(nb_dimensions);
  //  for(auto n:input_vector_images_offsets) nb_dimensions += n.size();
  points.SetPointDimension(nb_dimensions);

  // Declare output image and iterator
  AllocateOutputImage(nb_dimensions);
  auto iter_output = output->GetBufferPointer();

  // Main loop: build 1) a set of nD points, and 2) a output 4D image
  for(auto i=0; i<nb_pixels; i++) {
    if (iter_mask.Get() != 0) {
      double * v = points.push_back(); // add a new point
      int x=0;
      for(auto & a:iter_images) {
        v[x] = a.Get();
        x++;
      }
      // Set the images
      auto it = iter_output;
      for(auto x=0; x<nb_dimensions; x++) {
        *it = v[x];
        it += output_offset;
      }
    }
    else {
      // default value for the image
      auto it = iter_output;
      for(auto x=0; x<nb_dimensions; x++) {
        *it = 0.0;
        it += output_offset;
      }
    }
    // iterates
    ++iter_mask;
    ++iter_output;
    for(auto & a:iter_images) ++a;
  }


  PostProcessing();

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansInputDataBuilder::SetValuesFromVectorImage(const std::vector<PixelType*> & iter_vector,
                                                           double * v,
                                                           int & x)
{
  for(auto i=0; i<iter_vector.size(); i++) { // for all vector images
    auto & offsets = input_vector_images_offsets[i]; // get the list of offsets
    auto & iter = iter_vector[i]; // Consider the current pointer
    // DDS(offsets);
    for(auto n:offsets) { // loop on the nb of dimension to get
      auto p = iter+n; // jump with offset
      v[x] = *p;
      x++; // increment current vector
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansInputDataBuilder::AllocateOutputImage(int nb_dimensions)
{
  if (input_images.size() > 0) {
    auto input = input_images[0];
    return AllocateOutputImageFromT<ImageType>(nb_dimensions, input);
  }
  auto input = input_vector_images[0];
  AllocateOutputImageFromT<Image4DType>(nb_dimensions, input);}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansInputDataBuilder::PostProcessing()
{
  DDF();
  int N = points.GetNumberOfPoints();
  int D = points.GetNumberOfDimensions();
  DD(N);
  DD(D);

  IteratorType iter_mask(mask, mask->GetLargestPossibleRegion());
  auto iter_output = output->GetBufferPointer();
  int nb_pixels = mask->GetLargestPossibleRegion().GetNumberOfPixels();
  DD(nb_pixels);
  int output_offset = nb_pixels;
  DD(output_offset);

  // Find min and max
  std::vector<double> mins;
  std::vector<double> maxs;
  points.GetMinMax(mins, maxs);
  DDS(mins);
  DDS(maxs);

  // Clamp to min/max
  DD("Loop image 1");
  double max = 1.0;
  double min = 0.0;
  points.Rescale(mins, maxs, min, max);
  for(auto i=0; i<nb_pixels; i++) {
    if (iter_mask.Get() != 0) {
      auto it = iter_output;
      for(auto j=0; j<D; j++) {
        double v = *it;
        v = Rescale(v, mins[j], maxs[j], min, max);
        *it = v;
        it += output_offset;
      }
    }
    ++iter_mask;
    ++iter_output;
  }
  points.GetMinMax(mins, maxs);
  DDS(mins);
  DDS(maxs);
  syd::WriteImage<Image4DType>(output, "step1.mhd");

  // Compute some stats: median, mean, med-deviation
  std::vector<double> medians;
  std::vector<double> means;
  std::vector<double> mads;
  points.ComputeMedians(medians);
  points.ComputeMeans(means);
  points.ComputeMedianAbsDeviations(medians, mads);
  DDS(medians);
  DDS(means);
  DDS(mads);

  // initialize to compute min/max
  for(auto j=0; j<D; j++) {
    mins[j] = std::numeric_limits<double>::max();
    maxs[j] = std::numeric_limits<double>::lowest();
  }

  DD("Loop image 2");
  double lambda = 0.5;
  double outliers = 0;
  iter_mask.GoToBegin();
  iter_output = output->GetBufferPointer();
  for(auto i=0; i<nb_pixels; i++) {
    if (iter_mask.Get() != 0) {
      auto it = iter_output;
      for(auto j=0; j<D; j++) {
        double v = *it;
        double z_score = 0.6745 * (v - means[j]) / mads[j];
        if (z_score > 3.5) {
          iter_mask.Set(0.0); // remove points
          ++outliers;
        }
        else {
          BoxCoxTransform(v, lambda);
          if (isnan(v)) {
            iter_mask.Set(0);
            ++outliers;
          }
          else {
            *it = v;
            if (v < mins[j]) mins[j] = v;
            if (v > maxs[j]) maxs[j] = v;
          }
        }
        it += output_offset;
      }
    }
    ++iter_mask;
    ++iter_output;
  }
  syd::WriteImage<Image4DType>(output, "step2.mhd");
  DD(outliers);
  DDS(mins);
  DDS(maxs);

  DD("loop image 3");
  iter_mask.GoToBegin();
  iter_output = output->GetBufferPointer();
  points.clear();
  for(auto i=0; i<nb_pixels; i++) {
    if (iter_mask.Get() != 0) {
      double * point = points.push_back();
      auto it = iter_output;
      for(auto j=0; j<D; j++) {
        double v = *it;
        v = syd::Rescale(v, mins[j], maxs[j], 0.0, 1.0);
        point[j] = v;
        *it = v;
        it += output_offset;
      }
    }
    ++iter_mask;
    ++iter_output;
  }
  points.GetMinMax(mins, maxs);
  DDS(mins);
  DDS(maxs);

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansInputDataBuilder::PreProcessing()
{
  typedef itk::MinimumMaximumImageCalculator <ImageType> ImageCalculatorFilterType;
  ImageCalculatorFilterType::Pointer imageCalculatorFilter
    = ImageCalculatorFilterType::New ();
  imageCalculatorFilter->SetImage(input_images[0]);
  imageCalculatorFilter->Compute();
  DD(imageCalculatorFilter->GetMaximum());
  DD(imageCalculatorFilter->GetMinimum());

  /*
    for(auto & im:input_images) {
    typedef  itk::AdaptiveHistogramEqualizationImageFilter< ImageType > AdaptiveHistogramEqualizationImageFilterType;
    AdaptiveHistogramEqualizationImageFilterType::Pointer adaptiveHistogramEqualizationImageFilter = AdaptiveHistogramEqualizationImageFilterType::New();
    adaptiveHistogramEqualizationImageFilter->SetInput(im);
    adaptiveHistogramEqualizationImageFilter->SetRadius(1);
    adaptiveHistogramEqualizationImageFilter->SetAlpha(0.4);
    adaptiveHistogramEqualizationImageFilter->SetBeta(0.4);
    adaptiveHistogramEqualizationImageFilter->Update();
    im = adaptiveHistogramEqualizationImageFilter->GetOutput();
    }
  */

  // Normalise such as zero mean and unit variance
  /*
    for(auto & im:input_images) {
    auto fr = itk::NormalizeImageFilter<ImageType, ImageType>::New();
    fr->SetInput(im);
    fr->Update();
    im = fr->GetOutput();
    }
  */
  /*
    double lambda = 0.1;
    for(auto & im:input_images) {
    itk::ImageRegionIterator<ImageType> iter(im, im->GetLargestPossibleRegion());
    iter.GoToBegin();
    while (!iter.IsAtEnd()) {
    double v = iter.Get();
    if (v<=0) v = 0; // FIXME warning for HU !
    else v = (pow(v,lambda)-1.0)/lambda;
    //      else v = log(v);
    iter.Set(v);
    ++iter;
    }
    //    auto fr = itk::LogImageFilter<ImageType, ImageType>::New();
    //fr->SetInput(im);
    //fr->Update();
    //im = fr->GetOutput();
    }
  */

  /*
    imageCalculatorFilter->SetImage(input_images[0]);
    imageCalculatorFilter->Compute();
    DD(imageCalculatorFilter->GetMaximum());
    DD(imageCalculatorFilter->GetMinimum());
  */

  // Set values between [0-1]
  /*
    for(auto & im:input_images) {
    auto fr = itk::RescaleIntensityImageFilter<ImageType>::New();
    fr->SetOutputMinimum(0);
    fr->SetOutputMaximum(1);
    fr->SetInput(im);
    fr->Update();
    im = fr->GetOutput();
    }
  */

  /*
    imageCalculatorFilter->SetImage(input_images[0]);
    imageCalculatorFilter->Compute();
    DD(imageCalculatorFilter->GetMaximum());
    DD(imageCalculatorFilter->GetMinimum());
  */

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansInputDataBuilder::InsertVectorImagesAsImages()
{
  DDF();

  for(auto i=0; i<input_vector_images.size(); i++) {
    auto & vimg = input_vector_images[i];
    auto offsets = input_vector_images_offsets[i];
    for(auto offset:offsets) {

      DD(offset);

      // Create a 3D image
      ImageType::Pointer image = ImageType::New();
      auto spacing = image->GetSpacing();
      for(auto i=0; i<3; i++) spacing[i] = vimg->GetSpacing()[i];
      image->SetSpacing(spacing);
      auto origin = image->GetOrigin();
      for(auto i=0; i<3; i++) origin[i] = vimg->GetOrigin()[i];
      image->SetOrigin(origin);
      auto region = image->GetLargestPossibleRegion();
      auto index = region.GetIndex();
      auto size = region.GetSize();
      for(auto i=0; i<3; i++) index[i] = vimg->GetLargestPossibleRegion().GetIndex()[i];
      for(auto i=0; i<3; i++) size[i] = vimg->GetLargestPossibleRegion().GetSize()[i];
      region.SetSize(size);
      region.SetIndex(index);
      image->SetRegions(region);
      image->Allocate();

      // Copy pixel
      PixelType *iter_vector_image = vimg->GetBufferPointer() + offset;
      IteratorType iter_output(image, image->GetLargestPossibleRegion());
      iter_output.GoToBegin();
      while (!iter_output.IsAtEnd()) {
        auto v = *iter_vector_image;
        iter_output.Set(v);
        ++iter_output;
        ++iter_vector_image;
      }
      syd::WriteImage<ImageType>(image, "vector.mhd");
      DD("add input");
      AddInput(image);
    }
  }

}
// --------------------------------------------------------------------
