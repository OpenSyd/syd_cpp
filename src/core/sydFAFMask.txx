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
#include "sydImageProjection.h"

#include <itkResampleImageFilter.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkIdentityTransform.h>
#include <itkBinaryThresholdImageFilter.h>

//--------------------------------------------------------------------
template<class ImageType2D, class ImageType3D>
typename ImageType2D::Pointer
syd::FAFMask(const ImageType3D * input_SPECT, const ImageType2D * input_planar)
{
  DDF();
  // Project the image.

  ImageProjection_Parameters p;
  p.projectionDimension = 1;
  p.flipProjectionFlag = false;
  p.meanFlag = false;
  auto projection = syd::Projection<ImageType3D, ImageType2D>(input_SPECT, p);

  syd::WriteImage<ImageType2D>(projection, "proj.mhd");


  //Resample the projected image like the planar image
  typedef itk::ResampleImageFilter<ImageType2D,ImageType2D> FilterType;
  typedef itk::LinearInterpolateImageFunction<ImageType2D, double> InterpolatorType;
  typedef itk::IdentityTransform<double, ImageType2D::ImageDimension> TransformType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(projection);
  filter->SetSize(input_planar->GetLargestPossibleRegion().GetSize());
  filter->SetOutputSpacing(input_planar->GetSpacing());
  filter->SetOutputOrigin(input_planar->GetOrigin());
  filter->SetDefaultPixelValue(0.0);
  filter->SetOutputDirection(input_planar->GetDirection());
  filter->SetTransform(TransformType::New());
  filter->SetInterpolator(InterpolatorType::New());
  filter->Update();

  syd::WriteImage<ImageType2D>(filter->GetOutput(), "proj_res.mhd");


  //Binarize to obtain the positive value
  typedef itk::BinaryThresholdImageFilter <ImageType2D, ImageType2D> ThresholdImageFilterType;
  typename ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
  thresholdFilter->SetInput(filter->GetOutput());
  thresholdFilter->SetLowerThreshold(1.0);
  thresholdFilter->SetOutsideValue(0);
  thresholdFilter->SetInsideValue(1);
  thresholdFilter->Update();

  return (thresholdFilter->GetOutput());
}
//--------------------------------------------------------------------

