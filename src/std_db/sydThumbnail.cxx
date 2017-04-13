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

//itk
#include "itkResampleImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkScaleTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkChangeInformationImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

//------------------------------------------------------------------
syd::Thumbnail::Thumbnail()
{
  window = 6000.0;
  level = 23.5;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::Thumbnail::computeThumbnail(const syd::Image::pointer image, const std::string& thumbnailPath)
{

  int sizeZ = image->size[2];
  unsigned int stepZ = 20;
  unsigned int sizeThumbnail = 100; // X,Y size of 1 thumbnail in px
  double scale = std::max(((double)image->size[0])/sizeThumbnail,((double)image->size[1])/sizeThumbnail);

  // Read the mhd
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  ImageType::Pointer itkImage = syd::ReadImage<ImageType>(image->GetAbsolutePath());

  //Set the origin to 0
  typedef itk::ChangeInformationImageFilter<ImageType> FilterType;
  FilterType::Pointer filterChangeInformation = FilterType::New(); 
  ImageType::PointType offsetPoint;
  offsetPoint[0] = 0;
  offsetPoint[1] = 0;
  offsetPoint[2] = 0;
  filterChangeInformation->SetInput(itkImage);
  filterChangeInformation->SetOutputOrigin(offsetPoint);
  filterChangeInformation->ChangeOriginOn();
  filterChangeInformation->Update();

  //Find the minimum
  typedef itk::MinimumMaximumImageCalculator<ImageType> ImageCalculatorFilterType;
  ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New ();
  imageCalculatorFilter->SetImage(filterChangeInformation->GetOutput());
  imageCalculatorFilter->ComputeMinimum();
  ImageType::PixelType minItkImage = imageCalculatorFilter->GetMinimum();

  //Resize the input image
  ImageType::SizeType resampleSize;
  resampleSize[0] = sizeThumbnail;
  resampleSize[1] = sizeThumbnail;
  resampleSize[2] = sizeZ;
  ImageType::SpacingType resampleSpacing;
  resampleSpacing[0] = itkImage->GetLargestPossibleRegion().GetSize()[1]*itkImage->GetSpacing()[0]/sizeThumbnail;
  resampleSpacing[1] = itkImage->GetLargestPossibleRegion().GetSize()[2]*itkImage->GetSpacing()[1]/sizeThumbnail;
  resampleSpacing[2] = itkImage->GetSpacing()[2];

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
  resample->SetOutputSpacing(itkImage->GetSpacing());
  resample->SetTransform(scaleTransform);
  resample->SetInterpolator(interpolator);
  resample->SetDefaultPixelValue(minItkImage);
  resample->Update();

  //Create the staked image
  typedef itk::Image<unsigned char,2> Image2DType;
  Image2DType::Pointer stackedImage = Image2DType::New();
  Image2DType::RegionType region;
  Image2DType::IndexType start;
  start[0] = 0;
  start[1] = 0;
  Image2DType::SizeType size;
  size[0] = sizeThumbnail;
  size[1] = sizeThumbnail*(std::floor(sizeZ/stepZ) +1) + 3*(std::floor(sizeZ/stepZ));
  region.SetSize(size);
  region.SetIndex(start);
  stackedImage->SetRegions(region);
  stackedImage->Allocate();
  stackedImage->FillBuffer(255);

  //Fill the stack image
  for (unsigned int i=0; i<(std::floor(sizeZ/stepZ) +1); ++i)
  {
    //Create the iterator for the stack image
    Image2DType::RegionType iteratorStackedRegion;
    Image2DType::IndexType iteratorStackedStart;
    iteratorStackedStart[0] = 0;
    iteratorStackedStart[1] = i*(sizeThumbnail+3);
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

      if (value <= (level - window/2))
        imageStackedIterator.Set(0);
      else if (value >= (level + window/2))
        imageStackedIterator.Set(255);
      else
        imageStackedIterator.Set((unsigned char)((value/window-(level-window/2)/window)*255));

      ++imageStackedIterator;
      ++imageResampleIterator;
    }
  }

  typedef  itk::ImageFileWriter<Image2DType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(thumbnailPath);
  writer->SetInput(stackedImage);
  writer->Update();

}
//------------------------------------------------------------------