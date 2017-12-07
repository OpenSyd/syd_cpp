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
#include "itkImageAlgorithm.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkMattesMutualInformationImageToImageMetric.h"

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
syd::RegisterPlanarSPECT(ImageType * inputPlanar,
                         const ImageType * inputSPECT,
                         double & output_y_translation)
{
  //Create output
  /*typename ImageType::Pointer output = ImageType::New();
    output->SetRegions(inputPlanar->GetLargestPossibleRegion());
    output->SetOrigin(inputPlanar->GetOrigin());
    output->SetSpacing(inputPlanar->GetSpacing());
    output->Allocate(); */

  //---Resample inputSPECT and inputAM like inputPlanar (same spacing)---
  // Instantiate the resampler. Wire in the transform and the interpolator.
  typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
  typename ResampleFilterType::Pointer resampleFilterSPECT = ResampleFilterType::New();
  // Instantiate the b-spline interpolator and set it as the third order for bicubic.
  typedef itk::BSplineInterpolateImageFunction<ImageType, double, double> InterpolatorType;
  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
  interpolator->SetSplineOrder(3);

  // Instantiate the transform and specify it should be the id transform.
  typedef itk::IdentityTransform<double, 2> TransformType;
  TransformType::Pointer transform = TransformType::New();
  transform->SetIdentity();

  resampleFilterSPECT->SetInput(inputSPECT);
  resampleFilterSPECT->SetTransform(transform);
  resampleFilterSPECT->SetInterpolator(interpolator);
  resampleFilterSPECT->SetOutputOrigin(inputSPECT->GetOrigin());
  resampleFilterSPECT->SetOutputSpacing(inputPlanar->GetSpacing());

  // Set the output size.
  typename ImageType::SizeType sizeSPECT, sizeAM;
  sizeSPECT[0] = inputSPECT->GetLargestPossibleRegion().GetSize(0)*inputSPECT->GetSpacing()[0]/inputPlanar->GetSpacing()[0];
  sizeSPECT[1] = inputSPECT->GetLargestPossibleRegion().GetSize(1)*inputSPECT->GetSpacing()[1]/inputPlanar->GetSpacing()[1];
  resampleFilterSPECT->SetSize(sizeSPECT);
  resampleFilterSPECT->Update();

  typename ImageType::Pointer SPECTresample = ImageType::New();
  SPECTresample = resampleFilterSPECT->GetOutput();

  //Center along x
  double spectCenter = SPECTresample->GetOrigin()[0] + SPECTresample->GetLargestPossibleRegion().GetSize(0)*SPECTresample->GetSpacing()[0]/2;
  typename ImageType::PointType originPlanar = inputPlanar->GetOrigin();
  originPlanar[0] += spectCenter - originPlanar[0] - inputPlanar->GetLargestPossibleRegion().GetSize(0)*inputPlanar->GetSpacing()[0]/2;
  inputPlanar->SetOrigin(originPlanar);

  //Compute correlation coeff for different y-offset between inputPlanar and SPECTResample
  typedef itk::MattesMutualInformationImageToImageMetric<ImageType, ImageType> MICoeffFilterType;
  double minCorrelation(itk::NumericTraits<double >::infinity());
  unsigned int minTranslation(0);
  for (unsigned int translation=1 ; translation < SPECTresample->GetLargestPossibleRegion().GetSize()[1] + inputPlanar->GetLargestPossibleRegion().GetSize()[1]; ++translation) {
    originPlanar[1] = SPECTresample->GetOrigin()[1] - inputPlanar->GetLargestPossibleRegion().GetSize()[1]*inputPlanar->GetSpacing()[1] + translation*inputPlanar->GetSpacing()[1];
    inputPlanar->SetOrigin(originPlanar);
    typename MICoeffFilterType::Pointer miCoeffFilter = MICoeffFilterType::New();
    miCoeffFilter->SetMovingImage(inputPlanar);
    miCoeffFilter->SetFixedImage(SPECTresample);
    miCoeffFilter->SetTransform(transform);
    miCoeffFilter->SetInterpolator(interpolator);
    typename ImageType::RegionType smallRegion = itk::ImageAlgorithm::EnlargeRegionOverBox(inputPlanar->GetLargestPossibleRegion(), inputPlanar, SPECTresample.GetPointer());
    miCoeffFilter->SetFixedImageRegion(smallRegion);
    miCoeffFilter->UseAllPixelsOn();
    miCoeffFilter->SetNumberOfHistogramBins(50);
    miCoeffFilter->ReinitializeSeed();
    miCoeffFilter->Initialize();
    if (miCoeffFilter->GetValue(transform->GetParameters()) < minCorrelation) {
      minCorrelation = miCoeffFilter->GetValue(transform->GetParameters());
      minTranslation = translation;
    }
  }
  output_y_translation = minTranslation;

  //Choose the best y-offset and resample the image to fit with planar image
  originPlanar[1] = SPECTresample->GetOrigin()[1] - inputPlanar->GetLargestPossibleRegion().GetSize()[1]*inputPlanar->GetSpacing()[1] + minTranslation*inputPlanar->GetSpacing()[1];
  inputPlanar->SetOrigin(originPlanar);

  return inputPlanar;
}
//--------------------------------------------------------------------
