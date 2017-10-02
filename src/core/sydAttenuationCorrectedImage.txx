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
#include "sydImageCrop.h"

#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkLinearInterpolateImageFunction.h"

//--------------------------------------------------------------------
template<class ImageType2D>
typename ImageType2D::Pointer
syd::AttenuationCorrectedImage(const ImageType2D * input_GM, const ImageType2D * input_AM)
{
  // Create an image with 2 connected components
  typename ImageType2D::Pointer geoMeanACSC = ImageType2D::New();
  geoMeanACSC->SetRegions(input_GM->GetLargestPossibleRegion());
  geoMeanACSC->SetDirection(input_GM->GetDirection());
  geoMeanACSC->SetOrigin(input_GM->GetOrigin());
  geoMeanACSC->SetSpacing(input_GM->GetSpacing());
  geoMeanACSC->Allocate();
  geoMeanACSC->FillBuffer(0);

  //default ratio
  double ratio = 4.168696975;

  typedef typename  ImageType2D::PixelType PixelType;
  typedef typename itk::LinearInterpolateImageFunction <ImageType2D, PixelType> ImageInterpolatorType;
  typename ImageInterpolatorType::Pointer imageInterpolator = ImageInterpolatorType::New ();
  imageInterpolator->SetInputImage(input_AM);

  itk::ImageRegionIterator<ImageType2D> geoMeanACSCIterator(geoMeanACSC,geoMeanACSC->GetLargestPossibleRegion());
  itk::ImageRegionConstIterator<ImageType2D> input_GMIterator(input_GM,input_GM->GetLargestPossibleRegion());
  while(!geoMeanACSCIterator.IsAtEnd()) {
    typename ImageType2D::PointType point;
    geoMeanACSC->TransformIndexToPhysicalPoint(geoMeanACSCIterator.GetIndex(), point);

    //If the physical point (not the voxel) is inside input_AM, multiply by the corresponding interpolated value in input_AM
    //Else multiply it by a ratio value 4.168696975
    if (imageInterpolator->IsInsideBuffer(point))
      geoMeanACSCIterator.Set(input_GMIterator.Get()*imageInterpolator->Evaluate(point));
    else
      geoMeanACSCIterator.Set(input_GMIterator.Get()*ratio);

    ++geoMeanACSCIterator;
    ++input_GMIterator;
  }

  return (geoMeanACSC);
}
//--------------------------------------------------------------------

