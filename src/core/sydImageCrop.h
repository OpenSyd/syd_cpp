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

#ifndef SYDIMAGECROP_H
#define SYDIMAGECROP_H

// syd
#include "sydImageUtils.h"

// itk
#include <itkRegionOfInterestImageFilter.h>
#include <itkAutoCropLabelMapFilter.h>
#include <itkResampleImageFilter.h>
#include <itkAffineTransform.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkStatisticsLabelObject.h>
#include <itkLabelImageToLabelMapFilter.h>
#include <itkLabelMapToLabelImageFilter.h>
#include <itkExtractImageFilter.h>


// --------------------------------------------------------------------
namespace syd {

  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  ResampleAndCropImageLike(const ImageType * input,
                           const itk::ImageBase<ImageType::ImageDimension> * like,
                           int interpolationType,
                           typename ImageType::PixelType defaultValue);

  /// Crop the image according to the size of the other image
  template<class ImageType>
  typename ImageType::Pointer
  CropImageLike(const ImageType * input,
                const itk::ImageBase<ImageType::ImageDimension> * like);

  /// Crop an image by removing border with pixel value lower than the given threshold
  template<class ImageType>
  typename ImageType::Pointer
  CropImageWithLowerThreshold(const ImageType * input, typename ImageType::PixelType lower_threshold);
  //--------------------------------------------------------------------

  //Crop an image reducing it by removing the last (3rd) dimension
  template<class PixelType>
  typename itk::Image<PixelType, 2>::Pointer
  RemoveThirdDimension(const itk::Image<PixelType, 3> * input);
#include "sydImageCrop.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
