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

  syd::WriteImage<ImageType>(input_images[0], "vector-norm.mhd");

  // FIXME mask exist ?

  // FIXME check images sizes must be the same

  // Declare mask iterator
  IteratorType iter_mask(mask, mask->GetLargestPossibleRegion());

  // Declare image iterator
  std::vector<IteratorType> iter_images;
  for(auto & im:input_images)
    iter_images.push_back(IteratorType(im, im->GetLargestPossibleRegion()));

  // Declare vector iterator
  std::vector<PixelType*> iter_vector_images;
  for(auto & im:input_vector_images)
    iter_vector_images.push_back(im->GetBufferPointer());

  // Move iterators to begin
  iter_mask.GoToBegin();
  for(auto & a:iter_images) a.GoToBegin();

  // Get nb of pixels
  int nb_pixels = mask->GetLargestPossibleRegion().GetNumberOfPixels();
  int output_offset = nb_pixels;

  // Get nb of dimensions
  nb_dimensions = input_images.size();
  //  for(auto n:input_vector_images_offsets) nb_dimensions += n.size();
  //points.SetPointDimension(nb_dimensions);

  // Declare output image and iterator
  AllocateOutputImage(nb_dimensions);
  auto iter_output = output->GetBufferPointer();

  // Main lomonop
  for(auto i=0; i<nb_pixels; i++) {

    if (iter_mask.Get() != 0) {

      // Get the data
      double * v = new double[nb_dimensions];
      int x=0;
      for(auto & a:iter_images) {
        v[x] = a.Get();
        x++;
      }
      //      SetValuesFromVectorImage(iter_vector_images, v, x);

      // Set the points
      //if (v[0] > 0.865089 and v[0] < 0.872584)  // FIXME
      points.push_back(v);

      // Set the images
      auto it = iter_output;
      for(auto x=0; x<nb_dimensions; x++) {
        *it = v[x];
        it += output_offset;
      }
    }
    // iterates
    ++iter_mask;
    ++iter_output;
    for(auto & a:iter_images) ++a;
    for(auto & a:iter_vector_images) ++a;
  }
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
void syd::KmeansInputDataBuilder::PreProcessing()
{
    typedef itk::MinimumMaximumImageCalculator <ImageType> ImageCalculatorFilterType;
    ImageCalculatorFilterType::Pointer imageCalculatorFilter
    = ImageCalculatorFilterType::New ();
    imageCalculatorFilter->SetImage(input_images[0]);
    imageCalculatorFilter->Compute();
    DD(imageCalculatorFilter->GetMaximum());
    DD(imageCalculatorFilter->GetMinimum());

  // Normalise such as zero mean and unit variance
  for(auto & im:input_images) {
    auto fr = itk::NormalizeImageFilter<ImageType, ImageType>::New();
    fr->SetInput(im);
    fr->Update();
    im = fr->GetOutput();
  }

    imageCalculatorFilter->SetImage(input_images[0]);
    imageCalculatorFilter->Compute();
    DD(imageCalculatorFilter->GetMaximum());
    DD(imageCalculatorFilter->GetMinimum());

  // Set values between [0-1]
  for(auto & im:input_images) {
    auto fr = itk::RescaleIntensityImageFilter<ImageType>::New();
    fr->SetOutputMinimum(0);
    fr->SetOutputMaximum(1);
    fr->SetInput(im);
    fr->Update();
    im = fr->GetOutput();
  }

    imageCalculatorFilter->SetImage(input_images[0]);
    imageCalculatorFilter->Compute();
    DD(imageCalculatorFilter->GetMaximum());
    DD(imageCalculatorFilter->GetMinimum());


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
        iter_output.Set(*iter_vector_image);
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
