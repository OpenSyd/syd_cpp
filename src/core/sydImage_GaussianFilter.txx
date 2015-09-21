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


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer GaussianFilter(const ImageType * input, double sigma_in_mm)
{
  typedef itk::RecursiveGaussianImageFilter<ImageType, ImageType> GaussianFilterType;
  std::vector<typename GaussianFilterType::Pointer> gaussianFilters;
  for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
    gaussianFilters.push_back(GaussianFilterType::New());
    gaussianFilters[i]->SetDirection(i);
    gaussianFilters[i]->SetOrder(GaussianFilterType::ZeroOrder);
    gaussianFilters[i]->SetNormalizeAcrossScale(false);
    gaussianFilters[i]->SetSigma(sigma_in_mm); // in millimeter !
    if (gaussianFilters.size() == 1) gaussianFilters[0]->SetInput(input); // first filter
    else gaussianFilters[i]->SetInput(gaussianFilters[i-1]->GetOutput());
  }
  // Go ! (last filter)
  typename GaussianFilterType::Pointer f = gaussianFilters[gaussianFilters.size()-1];
  f->Update();
  return f->GetOutput();
}
//--------------------------------------------------------------------
