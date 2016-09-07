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

// itk
#include <itkExtractImageFilter.h>

//--------------------------------------------------------------------
template<class ImageType, class SliceType>
void
syd::ExtractSlices(const ImageType * input,
                   int direction,
                   std::vector<typename SliceType::Pointer > & slices)
{
  typedef itk::ExtractImageFilter<ImageType, SliceType> ExtractImageFilterType;
  typename ExtractImageFilterType::Pointer extract;

  // nb slices
  auto region = input->GetLargestPossibleRegion();
  auto index = region.GetIndex();
  auto size = region.GetSize();
  int nb_slices = region.GetSize()[direction];
  int start = index[direction];
  size[direction] = 1.0;
  region.SetSize(size);
  for(int i=0; i<nb_slices; i++) {
    extract = ExtractImageFilterType::New();
    extract->SetInput(input);
    index[direction] = start + i;
    region.SetIndex(index);
    extract->SetExtractionRegion(region);
    extract->SetDirectionCollapseToSubmatrix();
    extract->Update();
    slices.push_back(extract->GetOutput());
  }
}
//--------------------------------------------------------------------
