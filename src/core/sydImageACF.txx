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
#include "sydAttenuationImage.h"

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
syd::ComputeImageACF(const InputImageType * input, const ACF_Parameters & p)
{
  // Create the output like the input (fill with 0)
  /*typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(input);
  duplicator->Update();
  typename InputImageType::Pointer attenuation = duplicator->GetModifiableOutput();
  attenuation->FillBuffer(0.0);

  //  Loop on energies
  for (unsigned int i=0; i < p.numberEnergySPECT; ++i) {
    // create a temp image
    typename DuplicatorType::Pointer duplicator2 = DuplicatorType::New();
    duplicator2->SetInputImage(input);
    duplicator2->Update();
    typename InputImageType::Pointer attenuationTemp = duplicator2->GetModifiableOutput();
    attenuationTemp->FillBuffer(0.0);

    // Compute the attenuation map for one energy
    typename itk::ImageRegionIterator<InputImageType>
      imageIteratorTemp(attenuationTemp, attenuationTemp->GetLargestPossibleRegion());
    typename itk::ImageRegionConstIterator<InputImageType>
      imageIteratorInput(input, input->GetLargestPossibleRegion());
    while(!imageIteratorTemp.IsAtEnd()) {
      double mu(0);
      if (imageIteratorInput.Get() >0) {
        mu = p.attenuationWaterSPECT[i] + p.attenuationWaterCT/(p.attenuationBoneCT - p.attenuationWaterCT) *
          (p.attenuationBoneSPECT[i] - p.attenuationWaterSPECT[i])/1000.0 * imageIteratorInput.Get();
      }
      else {
        mu = p.attenuationWaterSPECT[i] + (p.attenuationWaterSPECT[i] - p.attenuationAirSPECT[i])/1000.0 * imageIteratorInput.Get();
      }
      imageIteratorTemp.Set(mu);

      ++imageIteratorTemp;
      ++imageIteratorInput;
    }

    // Sum the temp image with weight and the attenuation map
    typedef itk::MultiplyImageFilter<InputImageType, InputImageType, InputImageType> MultiplyFilterType;
    typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();
    multiplyFilter->SetInput(attenuationTemp);
    multiplyFilter->SetConstant(p.weights[i]);

    typedef itk::AddImageFilter<InputImageType, InputImageType> AddFilterType;
    typename AddFilterType::Pointer addFilter = AddFilterType::New();
    addFilter->SetInput1(attenuation);
    addFilter->SetInput2(multiplyFilter->GetOutput());
    addFilter->Update();
    attenuation=addFilter->GetOutput();
  }

  // If the attenuation is <0, set it to 0
  typedef itk::ThresholdImageFilter <InputImageType> ThresholdImageFilterType;
  typename ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
  thresholdFilter->SetInput(attenuation);
  thresholdFilter->ThresholdBelow(0);
  thresholdFilter->SetOutsideValue(0);
  thresholdFilter->Update();
  attenuation = thresholdFilter->GetOutput(); */
  typename InputImageType::Pointer attenuation;
  attenuation = syd::AttenuationImage(input, p.numberEnergySPECT, p.attenuationWaterCT, p.attenuationBoneCT, p.attenuationAirSPECT, p.attenuationWaterSPECT, p.attenuationBoneSPECT, p.weights);

  // Sum attenuation along the 2nd dimension:
  auto projection = syd::Projection<InputImageType, OutputImageType>(attenuation, p.proj);

  // Prepare variables
  double spacing = attenuation->GetSpacing()[p.proj.projectionDimension];
  double tempValue = spacing/(2.0*10.0); // x10 because mm to cm, and /2 because mean attenuation

  // Multiply the projected attenuation map by variables
  typedef itk::MultiplyImageFilter<OutputImageType, OutputImageType, OutputImageType> MultiplyImageFilterType;
  typename MultiplyImageFilterType::Pointer multiplyImageFilterAM = MultiplyImageFilterType::New();
  multiplyImageFilterAM->SetInput(projection);
  multiplyImageFilterAM->SetConstant(tempValue);
  multiplyImageFilterAM->Update();

  // Exponential
  typedef itk::ExpImageFilter<OutputImageType, OutputImageType> ExpImageFilterType;
  typename ExpImageFilterType::Pointer expImageFilter = ExpImageFilterType::New();
  expImageFilter->SetInput(multiplyImageFilterAM->GetOutput());
  expImageFilter->Update();

  return(expImageFilter->GetOutput());
}
//--------------------------------------------------------------------
