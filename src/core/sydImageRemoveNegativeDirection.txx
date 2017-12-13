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

#include "sydImageRemoveNegativeDirection.h"

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
syd::ImageRemoveNegativeDirection(ImageType * input)
{
  // Get direction and check
  auto inputDirection = input->GetDirection();
  bool IsNegative = false;
  for(auto i=0; i<ImageType::ImageDimension; i++)
    if (inputDirection[i][i] < 0) IsNegative = true;
  if (!IsNegative) return input;

  // Get matrix
  typedef itk::AffineTransform<double, ImageType::ImageDimension> AffineTransformType;
  auto affineTransform = AffineTransformType::New();

  // Get param
  auto inputSize = input->GetLargestPossibleRegion().GetSize();
  auto inputOrigin = input->GetOrigin();
  auto inputSpacing = input->GetSpacing();
  for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
    if (inputDirection[i][i]<0) {
      inputOrigin[i] = (double)inputOrigin[i] - inputSpacing[i] * ((double)inputSize[i]-1.0);
      inputDirection[i][i] *= -1.0;
    }
  }

  // Resampler
  typedef itk::ResampleImageFilter<ImageType,ImageType>  ResampleFilterType;
  auto resampler = ResampleFilterType::New();
  resampler->SetInput(input);
  resampler->SetTransform( affineTransform );
  resampler->SetSize( inputSize );
  resampler->SetOutputSpacing( input->GetSpacing() );
  resampler->SetOutputOrigin( inputOrigin );
  resampler->SetOutputDirection( inputDirection);

  try {
    resampler->Update();
  } catch(itk::ExceptionObject) {
    std::cerr<<"Error in ImageRemoveNegativeDirection when resampling the image"<<std::endl;
  }
  return resampler->GetOutput();
}
//--------------------------------------------------------------------
