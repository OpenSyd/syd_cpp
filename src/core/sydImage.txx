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


// --------------------------------------------------------------------
template<class ImageType>
void WriteImage(typename ImageType::Pointer image, std::string filename)
{
  typedef itk::ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(filename.c_str());
  writer->SetInput(image);
  try { writer->Update(); }
  catch(itk::ExceptionObject & err) {
    LOG(FATAL) << "Error while writing image [" << filename << "]";
  }
}
//--------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer ReadImage(std::string filename)
{
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filename.c_str());
  try { reader->Update(); }
  catch(itk::ExceptionObject & err) {
    LOG(FATAL) << "Error while reading image [" << filename << "]";
  }
  return reader->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
std::string ComputeImageMD5(typename ImageType::Pointer image)
{
  unsigned int n = image->GetLargestPossibleRegion().GetNumberOfPixels();
  MD5 md5;
  md5.update((char*)image->GetBufferPointer(), n);
  md5.finalize();
  return md5.hexdigest();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer ComputeAverageImage(std::vector<std::string> & filenames)
{
  // Read image one after the other to limit a bit the used memory
  typename ImageType::Pointer im1 = syd::ReadImage<ImageType>(filenames[0]);
  VLOG(2) << "Start with  " << filenames[0];
  for(auto i=1; i<filenames.size(); i++) {
    VLOG(2) << "Adding " << filenames[i];
    typedef itk::AddImageFilter<ImageType> FilterType;
    typename ImageType::Pointer im2 = syd::ReadImage<ImageType>(filenames[i]);
    typename FilterType::Pointer filter = FilterType::New();
    filter->InPlaceOn();
    filter->SetInput1(im1);
    filter->SetInput2(im2);
    filter->Update();
    im1 = filter->GetOutput();
  }

  // Average
  VLOG(2) << "Average";
  typedef float PixelType;
  typedef itk::Image<PixelType,3> OutputImageType;
  typedef itk::MultiplyImageFilter<ImageType, OutputImageType, OutputImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  typedef itk::CastImageFilter<OutputImageType, ImageType> CastFilterType;
  typename CastFilterType::Pointer cast = CastFilterType::New();
  filter->SetInput(im1);
  float f = 1.0/(float)(filenames.size());
  filter->SetConstant(f);
  cast->SetInput(filter->GetOutput());
  cast->Update();
  im1 = cast->GetOutput();
  return im1;
}
//--------------------------------------------------------------------



//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer ResampleImageLike(const ImageType * input,
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
typename ImageType::Pointer StitchImages(const ImageType * s1, const ImageType * s2,
                                         double threshold_cumul, double skip_slices)
{
  // compute bounding box
  typename ImageType::IndexType index1 = s1->GetLargestPossibleRegion().GetIndex();
  typename ImageType::IndexType index2 = s2->GetLargestPossibleRegion().GetIndex();
  typename ImageType::IndexType last1 = s1->GetLargestPossibleRegion().GetIndex()+s1->GetLargestPossibleRegion().GetSize();
  typename ImageType::IndexType last2 = s2->GetLargestPossibleRegion().GetIndex()+s2->GetLargestPossibleRegion().GetSize();

  typename ImageType::PointType pstart1, pstart2, pend1, pend2;
  s1->TransformIndexToPhysicalPoint(index1, pstart1);
  s1->TransformIndexToPhysicalPoint(last1, pend1);
  s2->TransformIndexToPhysicalPoint(index2, pstart2);
  s2->TransformIndexToPhysicalPoint(last2, pend2);

  typename ImageType::PointType pstart3, pend3;
  for(auto i=0; i<3; i++) pstart3[i] = std::min(pstart1[i], pstart2[i]);
  for(auto i=0; i<3; i++) pend3[i] = std::max(pend1[i], pend2[i]);

  typename ImageType::IndexType start, end;
  s1->TransformPhysicalPointToIndex(pstart3, start);
  s1->TransformPhysicalPointToIndex(pend3, end);

  typename ImageType::RegionType region;
  typename ImageType::SizeType size;
  for(auto i=0; i<3; i++) size[i] = end[i] - start[i];
  region.SetSize(size);

  typename ImageType::Pointer output = ImageType::New();
  output->CopyInformation(s1);
  output->SetRegions(region);
  typename ImageType::PointType origin;
  for(auto i=0; i<3; i++) origin[i] = (pstart1[i] < pstart2[i] ? s1->GetOrigin()[i]:s2->GetOrigin()[i]);
  output->SetOrigin(origin);
  output->Allocate();

  // Resize 2 images like output
  typename ImageType::Pointer rs1 = syd::ResampleImageLike<ImageType>(s1, output, 0,0);
  typename ImageType::Pointer rs2 = syd::ResampleImageLike<ImageType>(s2, output, 0,0);

  if (0) {
    //typedef itk::ImageSliceConstIteratorWithIndex<ImageType> ConstIteratorType;
    //typedef itk::ImageSliceIteratorWithIndex<ImageType> IteratorType;

    typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
    typedef itk::ImageRegionIterator<ImageType> IteratorType;

    ConstIteratorType iter1(rs1,rs1->GetLargestPossibleRegion());
    ConstIteratorType iter2(rs2,rs2->GetLargestPossibleRegion());
    IteratorType itero(output,output->GetLargestPossibleRegion());
    iter1.GoToBegin();
    iter2.GoToBegin();
    itero.GoToBegin();


    double t = 500;
    while (!iter1.IsAtEnd()) {
      typename ImageType::PixelType v1 = iter1.Get();
      typename ImageType::PixelType v2 = iter2.Get();
      itero.Set(std::max(v1,v2));
      ++iter1; ++iter2; ++itero;
    }
  }


  {
    typedef itk::ImageSliceConstIteratorWithIndex<ImageType> ConstIteratorType;
    typedef itk::ImageSliceIteratorWithIndex<ImageType> IteratorType;
    ConstIteratorType iter1(rs1,rs1->GetLargestPossibleRegion());
    ConstIteratorType iter2(rs2,rs2->GetLargestPossibleRegion());
    IteratorType itero(output,output->GetLargestPossibleRegion());
    iter1.SetFirstDirection(0); iter1.SetSecondDirection(1);
    iter2.SetFirstDirection(0); iter2.SetSecondDirection(1);
    itero.SetFirstDirection(0); itero.SetSecondDirection(1);
    iter1.GoToBegin();
    iter2.GoToBegin();
    itero.GoToBegin();

    std::vector<double> cumul1(rs1->GetLargestPossibleRegion().GetSize()[2]);
    std::vector<double> cumul2(rs2->GetLargestPossibleRegion().GetSize()[2]);

    int i=0;
    while (!iter1.IsAtEnd()) {
      cumul1[i] = 0.0;
      while (!iter1.IsAtEndOfSlice()) {
        while (!iter1.IsAtEndOfLine()) {
          cumul1[i] += iter1.Get();
          ++iter1;
        }
        iter1.NextLine();
      }
      iter1.NextSlice(); ++i;
    }
    i=0;
    while (!iter2.IsAtEnd()) {
      cumul2[i] = 0.0;
      while (!iter2.IsAtEndOfSlice()) {
        while (!iter2.IsAtEndOfLine()) {
          cumul2[i] += iter2.Get();
          ++iter2;
        }
        iter2.NextLine();
      }
      iter2.NextSlice(); ++i;
    }

    bool start = false;
    int end=0;
    double t = threshold_cumul;//150000;
    //    std::ofstream os("a.txt");
    for(auto i=0; i<cumul1.size(); i++) {
      //os << cumul1[i] << " " << cumul2[i] << std::endl;
      if (cumul2[i] > t and start) end = i;
      if (cumul2[i] > t and !start) start = true;
    }
    //os.close();

    iter1.GoToBegin();
    iter2.GoToBegin();
    itero.GoToBegin();

    i = 0;
    int skip = skip_slices;
    while (!iter1.IsAtEnd()) {

      while (!iter1.IsAtEndOfSlice()) {
        while (!iter1.IsAtEndOfLine()) {
          if (i>end-skip) itero.Set(iter1.Get());
          else itero.Set(iter2.Get());
          ++iter1; ++iter2; ++itero;
        }
        iter1.NextLine(); iter2.NextLine(); itero.NextLine();
      }
      iter1.NextSlice(); iter2.NextSlice(); itero.NextSlice(); ++i;
    }
  }

  return output;
}
//--------------------------------------------------------------------
