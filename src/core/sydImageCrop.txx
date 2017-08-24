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

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer ResampleAndCropImageLike(const ImageType * input,
                                                     const itk::ImageBase<ImageType::ImageDimension> * like,
                                                     int interpolationType,
                                                     typename ImageType::PixelType defaultValue)
{
  typedef itk::ResampleImageFilter<ImageType, ImageType> FilterType;
  auto t = itk::AffineTransform<double, ImageType::ImageDimension>::New();
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetTransform(t);
  filter->SetSize(like->GetLargestPossibleRegion().GetSize());
  filter->SetOutputSpacing(like->GetSpacing());
  filter->SetOutputOrigin(like->GetOrigin());
  filter->SetDefaultPixelValue(defaultValue);
  filter->SetOutputDirection(like->GetDirection());

  typename itk::InterpolateImageFunction<ImageType>::Pointer interpolator;
  switch (interpolationType) {
  case 0: { // NearestNeighbor
    interpolator = itk::NearestNeighborInterpolateImageFunction<ImageType, double>::New();
    break;
  }
  case 1: { // Linear
    interpolator =  itk::LinearInterpolateImageFunction<ImageType, double>::New();
    break;
  }
  }
  filter->SetInput(input);
  filter->SetInterpolator(interpolator);
  filter->Update();
  return filter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer CropImageLike(const ImageType * input,
                                          const itk::ImageBase<ImageType::ImageDimension> * like)
{
  typename ImageType::IndexType start;
  input->TransformPhysicalPointToIndex(like->GetOrigin(), start);
  typename ImageType::SizeType size;
  for(auto i=0; i<3; i++) {
    size[i] = (int)ceil((double)like->GetLargestPossibleRegion().GetSize()[i]*
                        like->GetSpacing()[i]/input->GetSpacing()[i]);
    // Could not be larger than the initial image
    if (size[i] > input->GetLargestPossibleRegion().GetSize()[i]) {
      LOG(FATAL) << "Error while CropImageLike, computed size is larger than initial image: " << size;
    }
    size[i] = std::min(size[i], input->GetLargestPossibleRegion().GetSize()[i]);

    // Change starting index if the direction is negative
    if (input->GetDirection()[i][i] < 0) start[i] = start[i] - size[i];

  }
  typename ImageType::RegionType r;
  r.SetSize(size);
  r.SetIndex(start);
  typedef itk::RegionOfInterestImageFilter<ImageType,ImageType> CropFilterType;
  auto crop = CropFilterType::New();
  crop->SetInput(input);
  crop->SetRegionOfInterest(r);
  crop->Update();
  return crop->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
CropImageWithLowerThreshold(const ImageType * input, typename ImageType::PixelType lower_threshold)
{
  // Binarize the image: set all pixel lower than threshold to the same value
  auto binarizer = itk::BinaryThresholdImageFilter<ImageType, ImageType>::New();
  binarizer->SetInput(input);
  binarizer->SetLowerThreshold(lower_threshold);
  binarizer->SetOutsideValue(lower_threshold);

  // Convert to LabelMap
  static const unsigned int Dim = ImageType::ImageDimension;
  typedef float LabelType; // signed long ?
  typedef itk::StatisticsLabelObject< LabelType, Dim > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelMapType;
  typedef itk::LabelImageToLabelMapFilter<ImageType, LabelMapType> ImageToMapFilterType;
  typename ImageToMapFilterType::Pointer imageToLabelFilter = ImageToMapFilterType::New();
  imageToLabelFilter->SetBackgroundValue(lower_threshold);
  imageToLabelFilter->SetInput(binarizer->GetOutput());

  // AutoCrop
  typedef itk::AutoCropLabelMapFilter<LabelMapType> AutoCropFilterType;
  typename AutoCropFilterType::Pointer autoCropFilter = AutoCropFilterType::New();
  autoCropFilter->SetInput(imageToLabelFilter->GetOutput());

  // Convert to image to get the size
  typedef itk::LabelMapToLabelImageFilter<LabelMapType, ImageType> MapToImageFilterType;
  typename MapToImageFilterType::Pointer labelToImageFilter = MapToImageFilterType::New();
  labelToImageFilter->SetInput(autoCropFilter->GetOutput());
  labelToImageFilter->Update();
  auto m_labeImage = labelToImageFilter->GetOutput();

  // Final crop (of the initial image not binarized)
  typedef itk::RegionOfInterestImageFilter<ImageType, ImageType> CropFilterType;
  m_labeImage->SetRequestedRegion(m_labeImage->GetLargestPossibleRegion());
  typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  cropFilter->SetInput(input);
  cropFilter->SetRegionOfInterest(m_labeImage->GetLargestPossibleRegion());
  cropFilter->Update();

  // End
  return cropFilter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
typename itk::Image<PixelType, 2>::Pointer
RemoveThirdDimension(const itk::Image<PixelType, 3> * input)
{
  //Compute the region whithout the last (3rd) dimension
  typedef itk::Image<PixelType, 3> InputImageType;
  typedef itk::Image<PixelType, 2> OutputImageType;
  typename InputImageType::IndexType start;
  typename InputImageType::SizeType size;

  start[0] = input->GetLargestPossibleRegion().GetIndex()[0];
  start[1] = input->GetLargestPossibleRegion().GetIndex()[1];
  start[2] = input->GetLargestPossibleRegion().GetIndex()[2];
  size[0] = input->GetLargestPossibleRegion().GetSize()[0];
  size[1] = input->GetLargestPossibleRegion().GetSize()[1];
  size[2] = 0;
  typename InputImageType::RegionType region(start, size);

  // Crop without the last dimension
  typedef itk::ExtractImageFilter<InputImageType, OutputImageType> CropFilterType;
  typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  cropFilter->SetInput(input);
  cropFilter->SetDirectionCollapseToIdentity();
  cropFilter->SetExtractionRegion(region);
  cropFilter->Update();

  // End
  return cropFilter->GetOutput();
}
//--------------------------------------------------------------------
