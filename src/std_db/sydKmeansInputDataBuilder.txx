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

// --------------------------------------------------------------------
template<class T>
void KmeansInputDataBuilder::AllocateOutputImageFromT(int nb_dimensions,
                                                      typename T::Pointer input)
{
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
