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
syd::AttenuationCorrectedPlanarImage(const ImageType2D * input_GM,
                                     const ImageType2D * input_ACF,
                                     double outside_factor)
{
  // Resample ACF like GM (images must be in the same frame of reference)
  // Use the region of GM, put -1 outside
  typedef itk::ResampleImageFilter<ImageType2D,ImageType2D> FilterType;
  typedef itk::LinearInterpolateImageFunction<ImageType2D, double> InterpolatorType;
  typedef itk::IdentityTransform<double, 2> TransformType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(input_ACF);
  filter->SetSize(input_GM->GetLargestPossibleRegion().GetSize());
  filter->SetOutputSpacing(input_GM->GetSpacing());
  filter->SetOutputOrigin(input_GM->GetOrigin());
  filter->SetDefaultPixelValue(-1.0);
  filter->SetOutputDirection(input_GM->GetDirection());
  filter->SetTransform(TransformType::New());
  filter->SetInterpolator(InterpolatorType::New());
  filter->Update();
  auto resampled_ACF = filter->GetOutput();

  syd::WriteImage<ImageType2D>(input_ACF, "input_acf.mhd");
  syd::WriteImage<ImageType2D>(input_GM, "input_gm.mhd");
  syd::WriteImage<ImageType2D>(resampled_ACF, "resampled_acf.mhd");

  // Create an image with the same size than GM
  typename ImageType2D::Pointer geoMeanACSC = ImageType2D::New();
  geoMeanACSC->SetRegions(input_GM->GetLargestPossibleRegion());
  geoMeanACSC->SetDirection(input_GM->GetDirection());
  geoMeanACSC->SetOrigin(input_GM->GetOrigin());
  geoMeanACSC->SetSpacing(input_GM->GetSpacing());
  geoMeanACSC->Allocate();
  geoMeanACSC->FillBuffer(0);

   // Copy the values from GM. For values outside (-1), use the outside_factor
  itk::ImageRegionIterator<ImageType2D> geoMeanACSCIterator(geoMeanACSC,geoMeanACSC->GetLargestPossibleRegion());
  itk::ImageRegionConstIterator<ImageType2D> input_GMIterator(input_GM,input_GM->GetLargestPossibleRegion());
  itk::ImageRegionConstIterator<ImageType2D> input_resampledACFIterator(resampled_ACF,resampled_ACF->GetLargestPossibleRegion());
  while(!geoMeanACSCIterator.IsAtEnd()) {
    auto v = input_resampledACFIterator.Get();
    if (v != -1.0) geoMeanACSCIterator.Set(input_GMIterator.Get()*v);
    else geoMeanACSCIterator.Set(input_GMIterator.Get()*outside_factor);
    ++geoMeanACSCIterator;
    ++input_GMIterator;
    ++input_resampledACFIterator;
  }

  return (geoMeanACSC);
}
//--------------------------------------------------------------------
