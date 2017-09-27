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

#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>
#include <itkMultiplyImageFilter.h>
#include <itkAddImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkExpImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkIdentityTransform.h>

//--------------------------------------------------------------------
template<class InputImageType, class OutputImageType>
typename OutputImageType::Pointer
syd::Attenuation(const InputImageType * input, const OutputImageType * likeImage,
                 double numberEnergySPECT, double attenuationWaterCT, double attenuationBoneCT,
                 std::vector<double>& attenuationAirSPECT,
                 std::vector<double>& attenuationWaterSPECT,
                 std::vector<double>& attenuationBoneSPECT,
                 std::vector<double>& weight)
{
  //Create the output like the input (fill with 0)
  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(input);
  duplicator->Update();
  typename InputImageType::Pointer attenuation = duplicator->GetModifiableOutput();
  attenuation->FillBuffer(0.0);

  for (unsigned int i=0; i < numberEnergySPECT; ++i)
  {
    //create a temp image
    typename DuplicatorType::Pointer duplicator2 = DuplicatorType::New();
    duplicator2->SetInputImage(input);
    duplicator2->Update();
    typename InputImageType::Pointer attenuationTemp = duplicator2->GetModifiableOutput();
    attenuationTemp->FillBuffer(0.0);

    //Compute the attenuation map for one energy
    typename itk::ImageRegionIterator<InputImageType> imageIteratorTemp(attenuationTemp, attenuationTemp->GetLargestPossibleRegion());
    typename itk::ImageRegionConstIterator<InputImageType> imageIteratorInput(input, input->GetLargestPossibleRegion());
    while(!imageIteratorTemp.IsAtEnd())
    {

      double mu(0);
      if (imageIteratorInput.Get() >0)
      {
        mu = attenuationWaterSPECT[i] + attenuationWaterCT/(attenuationBoneCT - attenuationWaterCT) * (attenuationBoneSPECT[i] - attenuationWaterSPECT[i])/1000.0 * imageIteratorInput.Get();
      }
      else
      {
        mu = attenuationWaterSPECT[i] + (attenuationWaterSPECT[i] - attenuationAirSPECT[i])/1000.0 * imageIteratorInput.Get();
      }
      imageIteratorTemp.Set(mu);

      ++imageIteratorTemp;
      ++imageIteratorInput;
    }

    //Sum the temp image with weight and the attenuation map
    typedef itk::MultiplyImageFilter<InputImageType, InputImageType, InputImageType> MultiplyFilterType;
    typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();
    multiplyFilter->SetInput(attenuationTemp);
    multiplyFilter->SetConstant(weight[i]);

    typedef itk::AddImageFilter<InputImageType, InputImageType> AddFilterType;
    typename AddFilterType::Pointer addFilter = AddFilterType::New();
    addFilter->SetInput1(attenuation);
    addFilter->SetInput2(multiplyFilter->GetOutput());
    addFilter->Update();
    attenuation=addFilter->GetOutput();
  }

  //If the attenuation is <0, set it to 0
  typedef itk::ThresholdImageFilter <InputImageType> ThresholdImageFilterType;
  typename ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
  thresholdFilter->SetInput(attenuation);
  thresholdFilter->ThresholdBelow(0);
  thresholdFilter->SetOutsideValue(0);
  thresholdFilter->Update();
  attenuation = thresholdFilter->GetOutput();

  //Now attenuation is the attenuation coefficient map.
  //We want the attenuation factor map:
  //First we will resample it like the likeImage
  //We will sum it and compute an exponential

  //Resample like the likeImage
  //Restriction like image has an isospacing
  typename InputImageType::SpacingType outputSpacing;
  /*outputSpacing[0] = likeImage->GetSpacing()[0];
  outputSpacing[1] = attenuation->GetSpacing()[1];
  outputSpacing[2] = likeImage->GetSpacing()[1];*/
  for(unsigned int i=0; i<InputImageType::ImageDimension; i++)
    outputSpacing[i] = likeImage->GetSpacing()[0];

  typename InputImageType::PointType outputOrigin;
  for(unsigned int i=0; i<InputImageType::ImageDimension; i++) {
    outputOrigin[i] = attenuation->GetOrigin()[i];
    outputOrigin[i] -= 0.5 * attenuation->GetSpacing()[i];
    outputOrigin[i] += 0.5 * outputSpacing[i];
  }

  typename InputImageType::SizeType outputSize;
  for(unsigned int i=0; i<InputImageType::ImageDimension; i++)
    outputSize[i] = (int)floor(attenuation->GetLargestPossibleRegion().GetSize()[i]*attenuation->GetSpacing()[i]/outputSpacing[i]);

  typedef itk::ResampleImageFilter<InputImageType,InputImageType> FilterType;
  typedef itk::LinearInterpolateImageFunction<InputImageType, double> InterpolatorType;
  typedef itk::IdentityTransform<double, 3> TransformType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(attenuation);
  filter->SetSize(outputSize);
  filter->SetOutputSpacing(outputSpacing);
  filter->SetOutputOrigin(outputOrigin);
  filter->SetDefaultPixelValue(0.0);
  filter->SetOutputDirection(attenuation->GetDirection());
  filter->SetTransform(TransformType::New());
  filter->SetInterpolator(InterpolatorType::New());
  filter->Update();

  //Sum attenuation along the 2nd dimension:
  int projectionDimension = 1;
  auto projection = syd::Projection<InputImageType, OutputImageType>(filter->GetOutput(), projectionDimension, false, true);

  //Prepare variables
  double size = filter->GetOutput()->GetLargestPossibleRegion().GetSize()[projectionDimension];
  double spacing = filter->GetOutput()->GetSpacing()[projectionDimension];
  double tempValue = spacing/(2.0*10.0);

  //Multiply the projected attenuation map by variables
  typedef itk::MultiplyImageFilter<OutputImageType, OutputImageType, OutputImageType> MultiplyImageFilterType;
  typename MultiplyImageFilterType::Pointer multiplyImageFilterAM = MultiplyImageFilterType::New();
  multiplyImageFilterAM->SetInput(projection);
  multiplyImageFilterAM->SetConstant(tempValue);
  multiplyImageFilterAM->Update();

  //Exponential
  typedef itk::ExpImageFilter<OutputImageType, OutputImageType> ExpImageFilterType;
  typename ExpImageFilterType::Pointer expImageFilter = ExpImageFilterType::New();
  expImageFilter->SetInput(multiplyImageFilterAM->GetOutput());
  expImageFilter->Update();

  return(expImageFilter->GetOutput());
}
//--------------------------------------------------------------------

