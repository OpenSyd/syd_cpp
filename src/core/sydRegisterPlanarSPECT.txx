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
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
syd::RegisterPlanarSPECT(const ImageType * inputPlanar, const ImageType * inputSPECT,
                 const ImageType * inputAM)
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
  typename ResampleFilterType::Pointer resampleFilterAM = ResampleFilterType::New();

  // Instantiate the b-spline interpolator and set it as the third order for bicubic.
  typedef itk::BSplineInterpolateImageFunction<ImageType, double, double> InterpolatorType;
  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
  interpolator->SetSplineOrder(3);

  // Instantiate the transform and specify it should be the id transform.
  typedef itk::IdentityTransform<double, 2> TransformType;
  TransformType::Pointer transform = TransformType::New();
  transform->SetIdentity();

  resampleFilterSPECT->SetInput(inputSPECT);
  resampleFilterAM->SetInput(inputAM);
  resampleFilterSPECT->SetTransform(transform);
  resampleFilterAM->SetTransform(transform);
  resampleFilterSPECT->SetInterpolator(interpolator);
  resampleFilterAM->SetInterpolator(interpolator);
  resampleFilterSPECT->SetOutputOrigin(inputSPECT->GetOrigin());
  resampleFilterAM->SetOutputOrigin(inputAM->GetOrigin());
  resampleFilterSPECT->SetOutputSpacing(inputPlanar->GetSpacing());
  resampleFilterAM->SetOutputSpacing(inputPlanar->GetSpacing());

  // Set the output size as specified on the command line.
  typename ImageType::SizeType sizeSPECT, sizeAM;
  sizeSPECT[0] = inputSPECT->GetLargestPossibleRegion().GetSize(0)*inputSPECT->GetSpacing()[0]/inputPlanar->GetSpacing()[0];
  sizeSPECT[1] = inputSPECT->GetLargestPossibleRegion().GetSize(1)*inputSPECT->GetSpacing()[1]/inputPlanar->GetSpacing()[1];
  sizeAM[0] = inputAM->GetLargestPossibleRegion().GetSize(0)*inputAM->GetSpacing()[0]/inputPlanar->GetSpacing()[0];
  sizeAM[1] = inputAM->GetLargestPossibleRegion().GetSize(1)*inputAM->GetSpacing()[1]/inputPlanar->GetSpacing()[1];
  resampleFilterSPECT->SetSize(sizeSPECT);
  resampleFilterAM->SetSize(sizeAM);
  resampleFilterSPECT->Update();
  resampleFilterAM->Update();

  typename ImageType::Pointer SPECTresample = ImageType::New();
  typename ImageType::Pointer AMresample = ImageType::New();
  SPECTresample = resampleFilterSPECT->GetOutput();
  AMresample = resampleFilterAM->GetOutput();

  //Center along x

  double planarCenter = inputPlanar->GetOrigin()[0] + inputPlanar->GetLargestPossibleRegion().GetSize(0)*inputPlanar->GetSpacing()[0]/2;
  typename ImageType::PointType originSPECT = SPECTresample->GetOrigin();
  originSPECT[0] += planarCenter - originSPECT[0] - SPECTresample->GetLargestPossibleRegion().GetSize(0)*SPECTresample->GetSpacing()[0]/2;
  SPECTresample->SetOrigin(originSPECT);
  typename ImageType::PointType originAM = AMresample->GetOrigin();
  originAM[0] += planarCenter - originAM[0] - AMresample->GetLargestPossibleRegion().GetSize(0)*AMresample->GetSpacing()[0]/2;
  AMresample->SetOrigin(originAM);

  //Compute correlation coeff for different y-offset between inputPlanar and inputSPECT
  //typedef itk::NormalizedCorrelationImageToImageMetric<ImageType, ImageType> CorrelationCoeffFilterType;
  typedef itk::NormalizedCorrelationImageToImageMetric<ImageType, ImageType> CorrelationCoeffFilterType;
  double maxCorrelation(-1);
  unsigned int maxTranslation(0);
  for (unsigned int translation=0 ; translation < inputPlanar->GetLargestPossibleRegion().GetSize()[1] - SPECTresample->GetLargestPossibleRegion().GetSize()[1] +1; ++translation)
  {
    originSPECT[1] = inputPlanar->GetOrigin()[1] + translation;
    SPECTresample->SetOrigin(originSPECT);
    typename CorrelationCoeffFilterType::Pointer correlationCoeffFilter = CorrelationCoeffFilterType::New();
    correlationCoeffFilter->SetMovingImage(SPECTresample);
    correlationCoeffFilter->SetFixedImage(inputPlanar);
    correlationCoeffFilter->SetTransform(transform);
    correlationCoeffFilter->SetInterpolator(interpolator);
    typename ImageType::RegionType smallRegion;
    smallRegion = inputPlanar->GetLargestPossibleRegion();
    smallRegion.Crop(SPECTresample->GetLargestPossibleRegion());
    correlationCoeffFilter->SetFixedImageRegion(smallRegion);
    correlationCoeffFilter->Initialize();
    correlationCoeffFilter->UseAllPixelsOn();
    std::cout << correlationCoeffFilter->GetValue(transform->GetParameters()) << std::endl;
    if (correlationCoeffFilter->GetValue(transform->GetParameters()) > maxCorrelation) maxTranslation = translation;
  }

  //Choose the best y-offset and resample the image to fit with planar image
  originAM[1] = inputPlanar->GetOrigin()[1] + maxTranslation - inputSPECT->GetOrigin()[1] + inputAM->GetOrigin()[1]; //Remove gap between SPECT and CT
  AMresample->SetOrigin(originAM);

  typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
  resampleFilter->SetTransform(transform.GetPointer());
  resampleFilter->SetInput(AMresample);
  resampleFilter->SetOutputSpacing(inputPlanar->GetSpacing());
  resampleFilter->SetOutputOrigin(inputPlanar->GetOrigin());
  resampleFilter->SetSize(inputPlanar->GetLargestPossibleRegion().GetSize());
  resampleFilter->Update();

  return resampleFilter->GetOutput();

}
//--------------------------------------------------------------------

