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

// syd
#include "sydThumbnail.h"
#include "sydImageUtils.h"
#include "sydStandardDatabase.h"
#include "sydDicomSerieHelper.h"

//itk
#include "itkResampleImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkScaleTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkChangeInformationImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

//------------------------------------------------------------------
syd::Thumbnail::Thumbnail()
{
  m_window = 6000.0;
  m_level = 23.5;
  m_thumbnailPath = "";
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::Thumbnail::computeThumbnail()
{
  //Create the thumbnail
  computeThumbnail(m_image, m_thumbnailPath);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::Thumbnail::setInputImage(const syd::DicomSerie::pointer images)
{
  // Convert the dicom to a mhd
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  m_image = syd::ReadDicomSerieImage<ImageType>(images);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::Thumbnail::setInputImage(const syd::Image::pointer image)
{
  // Read the mhd
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  m_image = syd::ReadImage<ImageType>(image->GetAbsolutePath());
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::Thumbnail::setThumbnailPath(const std::string& thumbnailPath)
{
  m_thumbnailPath = thumbnailPath;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::Thumbnail::computeThumbnail(const itk::Image<float,3>::Pointer image, const std::string& thumbnailPath)
{
  //Set the origin to 0
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef itk::ChangeInformationImageFilter<ImageType> FilterType;
  FilterType::Pointer filterChangeInformation = FilterType::New();
  ImageType::PointType offsetPoint;
  offsetPoint.Fill(0);
  ImageType::DirectionType directionVector;
  directionVector.Fill(0);
  directionVector[0][0] = 1;
  directionVector[1][1] = 1;
  directionVector[2][2] = 1;
  filterChangeInformation->SetInput(image);
  filterChangeInformation->SetOutputOrigin(offsetPoint);
  filterChangeInformation->ChangeOriginOn();
  filterChangeInformation->SetOutputDirection(directionVector);
  filterChangeInformation->ChangeDirectionOn();
  filterChangeInformation->Update();

  //Find the minimum
  typedef itk::StatisticsImageFilter<ImageType> ImageCalculatorFilterType;
  ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New ();
  imageCalculatorFilter->SetInput(filterChangeInformation->GetOutput());
  imageCalculatorFilter->Update();
  ImageType::PixelType minItkImage = imageCalculatorFilter->GetMinimum();
  ImageType::PixelType maxItkImage = imageCalculatorFilter->GetMaximum();
  ImageType::PixelType meanItkImage = imageCalculatorFilter->GetMean();

  //Modify Window/Level
  m_level = (maxItkImage + minItkImage)/2.0;
  m_window = maxItkImage - minItkImage;

  //Resize the input image
  int sizeZ = image->GetLargestPossibleRegion().GetSize()[2];
  unsigned int stepZ = 20;
  unsigned int sizeThumbnail = 100; // X,Y size of 1 thumbnail in px
  double scale = std::max(((double)image->GetLargestPossibleRegion().GetSize()[0])/sizeThumbnail,((double)image->GetLargestPossibleRegion().GetSize()[1])/sizeThumbnail);
  int nbImageZ = std::floor((sizeZ-1)/stepZ)+1;
  ImageType::SizeType resampleSize;
  resampleSize[0] = sizeThumbnail;
  resampleSize[1] = sizeThumbnail;
  resampleSize[2] = sizeZ;
  ImageType::SpacingType resampleSpacing;
  resampleSpacing[0] = image->GetLargestPossibleRegion().GetSize()[1]*image->GetSpacing()[0]/sizeThumbnail;
  resampleSpacing[1] = image->GetLargestPossibleRegion().GetSize()[2]*image->GetSpacing()[1]/sizeThumbnail;
  resampleSpacing[2] = image->GetSpacing()[2];

  //Scale the input image
  typedef itk::ScaleTransform<double, 3> ScaleTransformType;
  ScaleTransformType::Pointer scaleTransform = ScaleTransformType::New();
  ScaleTransformType::ParametersType parameters = scaleTransform->GetParameters();
  parameters[0] = scale;
  parameters[1] = scale;
  parameters[2] = 1;
  ImageType::PointType centralPoint;
  centralPoint[0] = 0;
  centralPoint[1] = 0;
  centralPoint[2] = 0;
  scaleTransform->SetParameters(parameters);
  scaleTransform->SetCenter(centralPoint);

  //Create the resample filter
  typedef itk::LinearInterpolateImageFunction<ImageType, double> LinearInterpolatorType;
  LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();
  typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleImageFilterType;
  ResampleImageFilterType::Pointer resample = ResampleImageFilterType::New();
  resample->SetInput(filterChangeInformation->GetOutput());
  resample->SetSize(resampleSize);
  resample->SetOutputSpacing(image->GetSpacing());
  resample->SetTransform(scaleTransform);
  resample->SetInterpolator(interpolator);
  resample->SetDefaultPixelValue(minItkImage);
  resample->Update();

  //Create the staked image (thumbnailLimitX thumbnails / line)
  typedef itk::Image<unsigned char,2> Image2DType;
  Image2DType::Pointer stackedImage = Image2DType::New();
  Image2DType::RegionType region;
  Image2DType::IndexType start;
  start[0] = 0;
  start[1] = 0;
  Image2DType::SizeType size;
  int nbDisplayX, nbDisplayY;
  int thumbnailLimitX = 4;
  if (nbImageZ < thumbnailLimitX) {
    nbDisplayX = nbImageZ;
    nbDisplayY = 1;
  }
  else {
    nbDisplayX = thumbnailLimitX;
    nbDisplayY = nbImageZ/thumbnailLimitX + 1;
  }
  size[0] = sizeThumbnail*nbDisplayX + 3*(nbDisplayX-1);
  size[1] = sizeThumbnail*nbDisplayY + 3*(nbDisplayY-1);
  region.SetSize(size);
  region.SetIndex(start);
  stackedImage->SetRegions(region);
  stackedImage->Allocate();
  stackedImage->FillBuffer(255);

  //Fill the stack image
  unsigned int i = 0;
  for (unsigned int y=0; y<nbDisplayY; ++y) {
    for (unsigned int x=0; x<nbDisplayX; ++x) {
      //Just check if it's not out of borders
      if (i*stepZ >= sizeZ)
        break;

      //Create the iterator for the stack image
      Image2DType::RegionType iteratorStackedRegion;
      Image2DType::IndexType iteratorStackedStart;
      iteratorStackedStart[0] = x*(sizeThumbnail+3);
      iteratorStackedStart[1] = y*(sizeThumbnail+3);
      Image2DType::SizeType iteratorStackedSize;
      iteratorStackedSize[0] = sizeThumbnail;
      iteratorStackedSize[1] = sizeThumbnail;
      iteratorStackedRegion.SetSize(iteratorStackedSize);
      iteratorStackedRegion.SetIndex(iteratorStackedStart);
      itk::ImageRegionIterator<Image2DType> imageStackedIterator(stackedImage, iteratorStackedRegion);

      //Create the iterator for the resize image
      ImageType::RegionType iteratorResampleRegion;
      ImageType::IndexType iteratorResampleStart;
      iteratorResampleStart[0] = 0;
      iteratorResampleStart[1] = 0;
      iteratorResampleStart[2] = i*stepZ;
      ImageType::SizeType iteratorResampleSize;
      iteratorResampleSize[0] = sizeThumbnail;
      iteratorResampleSize[1] = sizeThumbnail;
      iteratorResampleSize[2] = 1;
      iteratorResampleRegion.SetSize(iteratorResampleSize);
      iteratorResampleRegion.SetIndex(iteratorResampleStart);
      itk::ImageRegionConstIterator<ImageType> imageResampleIterator(resample->GetOutput(), iteratorResampleRegion);

      while(!imageStackedIterator.IsAtEnd())
      {
        // Set the current pixel into the stackedImage with the corresponding window/level value
        //if window = 0, the output value = 0
        double value = imageResampleIterator.Get();

        if (value <= (m_level - m_window/2))
          imageStackedIterator.Set(0);
        else if (value >= (m_level + m_window/2))
          imageStackedIterator.Set(255);
        else
          imageStackedIterator.Set((unsigned char)((value/m_window-(m_level-m_window/2)/m_window)*255));

        ++imageStackedIterator;
        ++imageResampleIterator;
      }
      ++i;
    }
  }
  for (unsigned int i=0; i<nbImageZ; ++i)
  {

  }

  typedef  itk::ImageFileWriter<Image2DType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(thumbnailPath);
  writer->SetInput(stackedImage);
  writer->Update();

}
//------------------------------------------------------------------