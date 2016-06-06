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
#include "itkRescaleIntensityImageFilter.h"

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
  DD(offset);
  DDS(indices);
  for(auto & i:indices) i = i*offset;
  DDS(indices);
  input_vector_images_offsets.push_back(indices);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansInputDataBuilder::BuildInputData()
{
  // FIXME preprocessing (gauss normalize)
  for(auto & im:input_images) {
    auto fr = itk::RescaleIntensityImageFilter<ImageType>::New();
    fr->SetOutputMinimum(0);
    fr->SetOutputMaximum(1);
    fr->SetInput(im);
    fr->Update();
    im = fr->GetOutput();
  }

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
  DD(nb_pixels);
  int output_offset = nb_pixels;

  // Get nb of dimensions
  nb_dimensions = input_images.size();
  DD(nb_dimensions);
  for(auto n:input_vector_images_offsets) nb_dimensions += n.size();
  DD(nb_dimensions);
  points.SetPointDimension(nb_dimensions);

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
      // DDV(v,x);
      SetValuesFromVectorImage(iter_vector_images, v, x);
      // DDV(v,x);

      // Set the points
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
  DD("end loop");
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
      v[x] = *p; // get the value
      x++; // increment current vector
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansInputDataBuilder::AllocateOutputImage(int nb_dimensions)
{
  auto input = input_images[0];
  output = Image4DType::New();
  auto spacing = output->GetSpacing();
  for(auto i=0; i<3; i++) spacing[i] = input->GetSpacing()[i];
  spacing[3] = 1.0;
  output->SetSpacing(spacing);
  auto origin = output->GetOrigin();
  for(auto i=0; i<3; i++) origin[i] = input->GetOrigin()[i];
  origin[3] = 0.0;
  output->SetOrigin(origin);
  auto region = output->GetLargestPossibleRegion();
  auto index = region.GetIndex();
  auto size = region.GetSize();
  for(auto i=0; i<3; i++) index[i] = input->GetLargestPossibleRegion().GetIndex()[i];
  for(auto i=0; i<3; i++) size[i] = input->GetLargestPossibleRegion().GetSize()[i];
  index[3] = 0;
  size[3] = nb_dimensions;
  region.SetSize(size);
  region.SetIndex(index);
  output->SetRegions(region);
  output->Allocate();
}
// --------------------------------------------------------------------
