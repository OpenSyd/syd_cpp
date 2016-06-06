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
  SetNumberOfClusters(3);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansFilter::SetInput(syd::NDimPoints::pointer p)
{
  points = p;
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
void syd::KmeansFilter::SetNumberOfClusters(int k)
{
  K = k;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::KmeansFilter::Run()
{
  DD("run");
  int n = points->GetNumberOfDimensions();
  if (n == 1) RunWithDim<1>();
  if (n == 2) RunWithDim<2>();
  if (n == 3) RunWithDim<3>();
  if (n == 4) RunWithDim<4>();
  if (n == 5) RunWithDim<5>();
  if (n > 5) {
    LOG(FATAL) << "error dim" << n;
    exit(0);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::KmeansFilter::ImageType::Pointer
syd::KmeansFilter::ComputeLabeledImage(syd::NDimPoints::pointer centers,
                                       Image4DType::Pointer input)
{
  // Check dim
  int N = points->GetNumberOfDimensions();
  DD(N);
  if (input->GetLargestPossibleRegion().GetSize()[3] != N) {
    LOG(FATAL) << "Error image dim 4 must be equal to " << N;
  }

  // Create and allocate output image
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

  // Fill labels
  itk::ImageRegionIterator<ImageType> oiter(output, output->GetLargestPossibleRegion());
  //itk::ImageRegionConstIterator<Image4DType> iter(input, input->GetLargestPossibleRegion());
  auto iter = input->GetBufferPointer();
  oiter.GoToBegin();
  double * p = new double[N];
  int offset = input->GetLargestPossibleRegion().GetSize()[0]*
    input->GetLargestPossibleRegion().GetSize()[1]*
    input->GetLargestPossibleRegion().GetSize()[2];
  DD(offset);
  while (!oiter.IsAtEnd()) {
    // Get the points
    auto titer = iter;
    for(auto i=0; i<N; i++) {
      p[i] = *titer;
      titer += offset;
    }
    // DDV(p, N);

    // Compute distance and labels
    double minValue = std::numeric_limits<double>::max();
    int minLabel = 0;
    int l=1;
    for(auto c=centers->begin(); c<centers->end(); c++) {
      double dist = 0.0;
      for(auto j=0; j<N; j++) dist += pow(p[j]-(*c)[j],2);
      if (dist < minValue) {
        minValue = dist;
        minLabel = l;
      }
      ++l;
    }
    oiter.Set(minLabel);

    // iterator
    ++iter;
    ++oiter;
  }

  return output;
}
// --------------------------------------------------------------------
