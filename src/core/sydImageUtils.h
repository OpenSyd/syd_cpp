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

#ifndef SYDIMAGEUTILS_H
#define SYDIMAGEUTILS_H

// syd
#include "sydCommon.h"
#include "sydFileUtils.h"
#include "sydDicomUtils.h"
#include "sydImageCrop.h"

// itk
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkAddImageFilter.h>
#include <itkMultiplyImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkOrientImageFilter.h>
#include <itkImageSliceIteratorWithIndex.h>
#include <itkImageSliceConstIteratorWithIndex.h>
#include <itkConvolutionImageFilter.h>
#include <itkJoinSeriesImageFilter.h>

// gdcm
#include "gdcmImageHelper.h"

// --------------------------------------------------------------------
namespace syd {

  //--------------------------------------------------------------------
  template<class PixelType>
    typename itk::Image<PixelType,3>::Pointer
    ReadDicomFromSingleFile(std::string filename);

  template<class PixelType>
    typename itk::Image<PixelType,3>::Pointer
    ReadDicomSerieFromFolder(std::string folder, std::string serie_uid);

  template<class PixelType>
    typename itk::Image<PixelType,3>::Pointer
    ReadDicomSerieFromListOfFiles(std::string folder, const std::vector<std::string> & files);

  template<class PixelType>
    void UpdateDicomImageInformation(typename itk::Image<PixelType,3>::Pointer image,
                                     const std::string & filename);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
    void WriteImage(const ImageType * image, std::string filename, int dimension = 3);

  void WriteImage(typename itk::ImageBase<3>::Pointer image, std::string filename);

  template<class ImageType>
    typename ImageType::Pointer ReadImage(std::string filename, int dimension = 3);

  itk::ImageIOBase::Pointer ReadImageHeader(const std::string & filename);

  template<unsigned int Dimension>
    typename itk::ImageBase<Dimension>::Pointer GetImageBase(const itk::ImageIOBase::Pointer & reader);

  template<class ImageType>
    typename ImageType::Pointer CreateImageLike(const typename itk::ImageBase<ImageType::ImageDimension> * like);

  template<class ImageType>
    typename ImageType::Pointer CreateImageLike(const itk::ImageIOBase * header);

  template<class ImageType>
    std::string ComputeImageMD5(typename ImageType::Pointer image);

  template<class ImageType>
    typename itk::Image<float, ImageType::ImageDimension>::Pointer CastImageToFloat(ImageType * input);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
    typename ImageType::Pointer
    ComputeAverageImage(std::vector<std::string> & filenames);

  template<class ImageType>
    typename ImageType::Pointer
    ComputeMeanFilterKernel(const typename ImageType::SpacingType & spacing, double radius);

  template<class ImageType, class MaskImageType>
    typename ImageType::PointType
    GetMaxPosition(const ImageType * input,
                   const MaskImageType * mask);

  template<class ImageType>
    void ScaleImage(ImageType * input, double scale);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType1, class ImageType2=ImageType1>
    bool ImagesHaveSameSize(const ImageType1 * a,
                            const ImageType2 * b);

  template<class ImageType1, class ImageType2=ImageType1>
    bool ImagesHaveSameSpacing(const ImageType1 * a,
                               const ImageType2 * b);

  template<class ImageType1, class ImageType2=ImageType1>
    bool ImagesHaveSameOrigin(const ImageType1 * a,
                              const ImageType2 * b);

  template<class ImageType1, class ImageType2=ImageType1>
    bool ImagesHaveSameSupport(const ImageType1 * a,
                               const ImageType2 * b);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  void CopyMHDImage(std::string from, std::string to, bool overwrite_if_exists);
  void RenameMHDImage(std::string from, std::string to, bool overwrite_if_exists);
  void RenameOrCopyMHDImage(std::string from, std::string to, bool erase, bool overwrite_if_exists);
  void DeleteMHDImage(std::string filename);
  std::string PointToString(const itk::Point<double,3> & t);
  //--------------------------------------------------------------------

#include "sydImageUtils.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
