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

#include "sydImageUtils.h"
#include "sydProjectionImage.h"
#include <itkMultiplyImageFilter.h>
#include <itkExpImageFilter.h>
#include <itkImageDuplicator.h>

//--------------------------------------------------------------------
template<class ImageType2D,class  ImageType3D>
typename ImageType2D::Pointer
syd::AttenuationCorrectedProjection(const ImageType2D * input_GM, const ImageType3D * input_AM,
                                    int dimension)
{
  //Prepare variables
  int size = input_AM->GetLargestPossibleRegion().GetSize(dimension);
  int spacing = input_AM->GetSpacing()[dimension];

  double tempValue = 0.2*spacing*size;

  //Project AM
  auto input_AMProjected = syd::Projection<ImageType3D, ImageType2D>(input_AM, dimension, 0, 1);

  //Multiply the projected AM by variables
  typedef itk::MultiplyImageFilter<ImageType2D, ImageType2D, ImageType2D> MultiplyImageFilterType;
  typename MultiplyImageFilterType::Pointer multiplyImageFilterAM = MultiplyImageFilterType::New();
  multiplyImageFilterAM->SetInput(input_AMProjected);
  multiplyImageFilterAM->SetConstant(tempValue);

  //Exponential
  typedef itk::ExpImageFilter<ImageType2D, ImageType2D> ExpImageFilterType;
  typename ExpImageFilterType::Pointer expImageFilter = ExpImageFilterType::New();
  expImageFilter->SetInput(multiplyImageFilterAM->GetOutput());

  //Multiply the exponential by GM
  typename MultiplyImageFilterType::Pointer multiplyImageFilterGM = MultiplyImageFilterType::New();
  multiplyImageFilterGM->SetInput1(input_GM);
  multiplyImageFilterGM->SetInput2(expImageFilter->GetOutput());

  return (multiplyImageFilterGM->GetOutput());
}
//--------------------------------------------------------------------

