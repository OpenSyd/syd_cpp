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

#include <itkStatisticsImageFilter.h>
#include <itkMultiplyImageFilter.h>

//--------------------------------------------------------------------
template<class ImageType2D, class ImageType3D>
typename ImageType3D::Pointer
syd::FAFCalibratedImage(const ImageType3D * input_SPECT, const ImageType2D * input_planar, const ImageType2D * input_mask)
{
  //Compute the total sum of the voxel in SPECT image
  typedef itk::StatisticsImageFilter<ImageType3D> StatisticsImage3DFilterType;
  typename StatisticsImage3DFilterType::Pointer statisticsSPECTFilter = StatisticsImage3DFilterType::New();
  statisticsSPECTFilter->SetInput(input_SPECT);
  statisticsSPECTFilter->Update();
  double SPECTSum = statisticsSPECTFilter->GetSum();

  //Compute the total sum voxel in the planar
  typedef itk::StatisticsImageFilter<ImageType2D> StatisticsImage2DFilterType;
  typename StatisticsImage2DFilterType::Pointer statisticsPlanarFilter = StatisticsImage2DFilterType::New();
  statisticsPlanarFilter->SetInput(input_planar);
  statisticsPlanarFilter->Update();
  double planarSum = statisticsPlanarFilter->GetSum();

  //Multiply the planar by the mask
  typedef itk::MultiplyImageFilter <ImageType2D, ImageType2D> MultiplyImage2DFilterType;
  typename MultiplyImage2DFilterType::Pointer multiplyFilter = MultiplyImage2DFilterType::New();
  multiplyFilter->SetInput1(input_planar);
  multiplyFilter->SetInput2(input_mask);
  multiplyFilter->Update();

  //Compute the total sum voxel in the planar mask
  typename StatisticsImage2DFilterType::Pointer statisticsMaskFilter = StatisticsImage2DFilterType::New();
  statisticsMaskFilter->SetInput(multiplyFilter->GetOutput());
  statisticsMaskFilter->Update();
  double MaskSum = statisticsMaskFilter->GetSum();

  //Compute the FAF value
  double faf = SPECTSum * MaskSum / planarSum;

  //Compute the FAF corrected image
  typedef itk::MultiplyImageFilter<ImageType3D, ImageType3D, ImageType3D> MultiplyImage3DFilterType;
  typename MultiplyImage3DFilterType::Pointer multiplySPECTFilter = MultiplyImage3DFilterType::New();
  multiplySPECTFilter->SetInput(input_SPECT);
  multiplySPECTFilter->SetConstant(faf);
  multiplySPECTFilter->Update();

  return (multiplySPECTFilter->GetOutput());
}
//--------------------------------------------------------------------

