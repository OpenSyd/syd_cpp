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
syd::AttenuationCorrectedProjection(const ImageType2D * input_GM, const ImageType2D * input_AM,
                                    const ImageType3D * input_AM_model, int dimension)
{
  //Prepare variables
  double size = input_AM_model->GetLargestPossibleRegion().GetSize(dimension);
  double spacing = input_AM_model->GetSpacing()[dimension];

  double tempValue = 0.05*spacing*size;

  //Multiply the projected AM by variables
  typedef itk::MultiplyImageFilter<ImageType2D, ImageType2D, ImageType2D> MultiplyImageFilterType;
  typename MultiplyImageFilterType::Pointer multiplyImageFilterAM = MultiplyImageFilterType::New();
  multiplyImageFilterAM->SetInput(input_AM);
  multiplyImageFilterAM->SetConstant(tempValue);

  //Exponential
  typedef itk::ExpImageFilter<ImageType2D, ImageType2D> ExpImageFilterType;
  typename ExpImageFilterType::Pointer expImageFilter = ExpImageFilterType::New();
  expImageFilter->SetInput(multiplyImageFilterAM->GetOutput());

  //Multiply the exponential by GM
  typename MultiplyImageFilterType::Pointer multiplyImageFilterGM = MultiplyImageFilterType::New();
  multiplyImageFilterGM->SetInput1(input_GM);
  multiplyImageFilterGM->SetInput2(expImageFilter->GetOutput());
  multiplyImageFilterGM->Update();

  return (multiplyImageFilterGM->GetOutput());
}
//--------------------------------------------------------------------

