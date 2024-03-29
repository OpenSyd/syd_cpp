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
#include "sydKmeansFilter.h"

// --------------------------------------------------------------------
syd::KmeansFilter::KmeansFilter()
{
  // SetNumberOfClusters(3);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// void syd::KmeansFilter::SetInput(syd::NDimPoints::pointer p)
// {
//   points = p;
// }
// --------------------------------------------------------------------

// --------------------------------------------------------------------
// void syd::KmeansFilter::SetNumberOfClusters(int k)
// {
//   K = k;
// }
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// void syd::KmeansFilter::Run()
// {
//   int n = points->GetNumberOfDimensions();
//   if (n == 1) RunWithDim<1>();
//   if (n == 2) RunWithDim<2>();
//   if (n == 3) RunWithDim<3>();
//   if (n == 4) RunWithDim<4>();
//   if (n == 5) RunWithDim<5>();
//   if (n == 6) RunWithDim<6>();
//   if (n == 7) RunWithDim<7>();
//   if (n > 7) {
//     LOG(FATAL) << "error dim " << n << " not enough template in sydKmeansFilter";
//     exit(0);
//   }
// }
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::KmeansFilter::ImageType::Pointer
syd::KmeansFilter::AllocateOutputImage(Image4DType::Pointer input)
{
  auto output = ImageType::New();

  auto spacing = output->GetSpacing();
  for(auto i=0; i<3; i++) spacing[i] = input->GetSpacing()[i];
  output->SetSpacing(spacing);

  auto origin = output->GetOrigin();
  for(auto i=0; i<3; i++) origin[i] = input->GetOrigin()[i];
  output->SetOrigin(origin);

  auto region = output->GetLargestPossibleRegion();
  auto index = region.GetIndex();
  auto size = region.GetSize();
  for(auto i=0; i<3; i++) index[i] = input->GetLargestPossibleRegion().GetIndex()[i];
  for(auto i=0; i<3; i++) size[i] = input->GetLargestPossibleRegion().GetSize()[i];
  region.SetSize(size);
  region.SetIndex(index);
  output->SetRegions(region);
  output->Allocate();

  return output;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::KmeansFilter::ImageType::Pointer
syd::KmeansFilter::ComputeLabeledImage(int K, // number of clusters
                                       int D, // number of dimensions
                                       double * centers,
                                       ImageType::Pointer mask,
                                       Image4DType::Pointer input)
{
  // Check dim
  if (input->GetLargestPossibleRegion().GetSize()[3] != D) {
    LOG(FATAL) << "Error image 4th dimension must be equal to " << D;
  }

  // Create and allocate output image
  auto output = AllocateOutputImage(input);

  // Fill labels
  itk::ImageRegionIterator<ImageType> oiter(output, output->GetLargestPossibleRegion());
  itk::ImageRegionIterator<ImageType> miter(mask, mask->GetLargestPossibleRegion());
  auto iter = input->GetBufferPointer();
  oiter.GoToBegin();
  miter.GoToBegin();
  double * p = new double[D];
  int offset = input->GetLargestPossibleRegion().GetSize()[0]*
    input->GetLargestPossibleRegion().GetSize()[1]*
    input->GetLargestPossibleRegion().GetSize()[2];
  while (!oiter.IsAtEnd()) {

    // Check if in the mask
    if (miter.Get() != 0) { // 0 is background

      // Get the points
      auto titer = iter;
      for(auto i=0; i<D; i++) {
        p[i] = *titer;
        titer += offset;
      }

      // Compute distance and labels
      double minValue = std::numeric_limits<double>::max();
      int minLabel = 0;
      int l=1;
      for(auto i=0; i<K; i++) { //c:*centers) {
        double dist = 0.0;
        for(auto j=0; j<D; j++) dist += pow(p[j]-centers[i*D+j],2);
        if (dist < minValue) {
          minValue = dist;
          minLabel = l;
        }
        ++l;
      }
      oiter.Set(minLabel);
    }
    else oiter.Set(0);

    // iterator
    ++iter;
    ++oiter;
    ++miter;
  }

  return output;
}
// --------------------------------------------------------------------
