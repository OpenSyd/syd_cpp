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
#include "itkFlipImageFilter.h"

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
syd::Flip(const ImageType * inputImage, int axis, bool flipOrigin)
{
  // Filter
  typedef itk::FlipImageFilter<ImageType> FlipImageFilterType;
  typename FlipImageFilterType::Pointer flipFilter=FlipImageFilterType::New();
  flipFilter->SetInput(inputImage);

  typename FlipImageFilterType::FlipAxesArrayType axes;
  axes.Fill(false);
  axes[axis]=true;
  flipFilter->SetFlipAxes(axes);
  flipFilter->SetFlipAboutOrigin(flipOrigin);

  flipFilter->Update();
  return flipFilter->GetOutput();
}
//--------------------------------------------------------------------
