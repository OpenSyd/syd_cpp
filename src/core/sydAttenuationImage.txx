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

#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>
#include <itkMultiplyImageFilter.h>
#include <itkAddImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkImageDuplicator.h>


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
syd::AttenuationImage(const ImageType * input, const double numberEnergySPECT,
                      const double attenuationWaterCT, const double attenuationBoneCT,
                      const std::vector<double>& attenuationAirSPECT,
                      const std::vector<double>& attenuationWaterSPECT,
                      const std::vector<double>& attenuationBoneSPECT,
                      const std::vector<double>& weight)
{
  //Create the output like the input (fill with 0)
  typedef itk::ImageDuplicator< ImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(input);
  duplicator->Update();
  typename ImageType::Pointer attenuation = duplicator->GetModifiableOutput();
  attenuation->FillBuffer(0.0);

  for (unsigned int i=0; i < numberEnergySPECT; ++i)
  {
    //create a temp image
    typename DuplicatorType::Pointer duplicator2 = DuplicatorType::New();
    duplicator2->SetInputImage(input);
    duplicator2->Update();
    typename ImageType::Pointer attenuationTemp = duplicator2->GetModifiableOutput();
    attenuationTemp->FillBuffer(0.0);

    //Compute the attenuation map for one energy
    typename itk::ImageRegionIterator<ImageType> imageIteratorTemp(attenuationTemp, attenuationTemp->GetLargestPossibleRegion());
    typename itk::ImageRegionConstIterator<ImageType> imageIteratorInput(input, input->GetLargestPossibleRegion());
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
    typedef itk::MultiplyImageFilter<ImageType, ImageType, ImageType> MultiplyFilterType;
    typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();
    multiplyFilter->SetInput(attenuationTemp);
    multiplyFilter->SetConstant(weight[i]);

    typedef itk::AddImageFilter<ImageType, ImageType> AddFilterType;
    typename AddFilterType::Pointer addFilter = AddFilterType::New();
    addFilter->SetInput1(attenuation);
    addFilter->SetInput2(multiplyFilter->GetOutput());
    addFilter->Update();
    attenuation=addFilter->GetOutput();
  }

  //If the attenuation is <0, set it to 0
  typedef itk::ThresholdImageFilter <ImageType> ThresholdImageFilterType;
  typename ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
  thresholdFilter->SetInput(attenuation);
  thresholdFilter->ThresholdBelow(0);
  thresholdFilter->SetOutsideValue(0);
  thresholdFilter->Update();
  attenuation = thresholdFilter->GetOutput();

  return attenuation;
}
//--------------------------------------------------------------------

