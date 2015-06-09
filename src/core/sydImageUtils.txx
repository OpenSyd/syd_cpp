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
template<unsigned int Dimension>
typename itk::ImageBase<Dimension>::Pointer
GetImageBase(const itk::ImageIOBase::Pointer & reader)
{
  if (reader->GetComponentSize() != 1) { // scalar image ?
    LOG(FATAL) << "Error, only use GetImageBase with scalar image.";
  }
  typedef itk::ImageBase<Dimension> BaseImageType;
  typename BaseImageType::Pointer i = BaseImageType::New();
  typename BaseImageType::RegionType region;
  typename BaseImageType::SizeType size;
  typename BaseImageType::IndexType start;
  typename BaseImageType::SpacingType spacing;
  typename BaseImageType::PointType origin;
  typename BaseImageType::DirectionType direction;
  for(auto i=0; i<Dimension; i++) {
    size[i] = reader->GetDimensions(i);//GetIORegion().GetSize()[i];
    start[i] = 0;//a->GetIORegion().GetIndex()[i];
    spacing[i] = reader->GetSpacing(i);
    origin[i] = reader->GetOrigin(i);
  }
  for(auto i=0; i<Dimension; i++) {
    auto v = reader->GetDirection(i);
    for(auto j=0; j<Dimension; j++) {
      direction(i,j) = v[j];
    }
  }
  region.SetSize(size);
  region.SetIndex(start);
  i->SetRegions(region);
  i->SetSpacing(spacing);
  i->SetOrigin(origin);
  i->SetDirection(direction);
  return i;
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
typename ImageType::Pointer CreateImageLike(typename itk::ImageBase<ImageType::ImageDimension>* like)
{
  typename ImageType::Pointer output = ImageType::New();
  output->CopyInformation(like);
  output->SetRegions(like->GetLargestPossibleRegion());
  output->Allocate();
  return output;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer ComputeAverageImage(std::vector<std::string> & filenames)
{
  // Read image one after the other to limit a bit the used memory
  typename ImageType::Pointer im1 = syd::ReadImage<ImageType>(filenames[0]);
  LOG(2) << "Start with  " << filenames[0];
  for(auto i=1; i<filenames.size(); i++) {
    LOG(2) << "Adding " << filenames[i];
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
  LOG(2) << "Average";
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

  // Check X and Y same size
  bool isCorrect = true;
  for(auto i=0; i<2; i++)  {
    if (pstart1[i] != pstart2[i]) isCorrect = false;
    if (pend1[i] != pend2[i]) isCorrect = false;
  }
  if (!isCorrect) {
    LOG(FATAL) << "Error could not stitch the two images because X/Y size are not equal: "
               << std::endl << "Image1: " << pstart1 << " -> " << pend1
               << std::endl << "Image2: " << pstart2 << " -> " << pend2;
  }

  // Compute the total region
  typename ImageType::PointType pstart3, pend3;
  for(auto i=0; i<3; i++)  {
    if (pstart1[i] > pend1[i]) pstart3[i] = std::max(pstart1[i], pstart2[i]); // reverse order
    if (pstart1[i] < pend1[i]) pstart3[i] = std::min(pstart1[i], pstart2[i]);

    if (pend1[i] > pstart1[i]) pend3[i] = std::max(pend1[i], pend2[i]);
    if (pend1[i] < pstart1[i]) pend3[i] = std::min(pend1[i], pend2[i]); // reverse order
  }

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
  for(auto i=0; i<3; i++) {
    if (pstart1[i] > pend1[i]) // reverse
      origin[i] = (pstart1[i] > pstart2[i] ? s1->GetOrigin()[i]:s2->GetOrigin()[i]);
    else
      origin[i] = (pstart1[i] < pstart2[i] ? s1->GetOrigin()[i]:s2->GetOrigin()[i]);
  }
  output->SetOrigin(origin);
  output->Allocate();

  // Resize 2 images like output
  typename ImageType::Pointer rs1 = syd::ResampleAndCropImageLike<ImageType>(s1, output, 0,0);
  typename ImageType::Pointer rs2 = syd::ResampleAndCropImageLike<ImageType>(s2, output, 0,0);

  // Swap if not correct order
  if (pstart3[2] == pstart1[2]) std::swap(rs1, rs2);

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

    // Cumulate pixel values along slices (profile)
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

    // Find the slice where cumul larger than threshold_cumul
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


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer ComputeMeanFilterKernel(const typename ImageType::SpacingType & spacing, double radius)
{
  // Some kind of cache to speed up a bit
  static std::map<double, typename ImageType::Pointer> cache;
  if (cache.find(radius) != cache.end()) {
    return cache.find(radius)->second;
  }

  // Compute a kernel that corresponds to a sphere with 1 inside, 0
  // outside and in between proportional to the intersection between
  // the pixel and the sphere. Computed by Monte-Carlo because I don't
  // know an equation that compute the intersection volume between a
  // box and a sphere ...
  auto kernel = ImageType::New();

  // Size of the kernel in pixel (minimum 3 pixels)
  typename ImageType::SizeType size;
  size[0] = std::max((int)ceil(radius*2/spacing[0]), 3);
  size[1] = std::max((int)ceil(radius*2/spacing[1]), 3);
  size[2] = std::max((int)ceil(radius*2/spacing[2]), 3);

  // Compute the region, such as the origin is at the center
  typename ImageType::IndexType start;
  start.Fill(0);
  typename ImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);
  kernel->SetRegions(region);
  kernel->SetSpacing(spacing);
  typename ImageType::PointType origin;
  origin[0] = -(double)size[0]/2.0*spacing[0]+spacing[0]/2.0;
  origin[1] = -(double)size[1]/2.0*spacing[1]+spacing[1]/2.0;
  origin[2] = -(double)size[2]/2.0*spacing[2]+spacing[2]/2.0;
  kernel->SetOrigin(origin);
  kernel->Allocate();

  // Fill the kernel
  itk::ImageRegionIteratorWithIndex<ImageType> iter(kernel, region);
  typename ImageType::PointType center;
  center.Fill(0.0);
  typename ImageType::PointType hh; // half a voxel
  hh[0] = spacing[0]/2.0;
  hh[1] = spacing[1]/2.0;
  hh[2] = spacing[2]/2.0;
  double h = hh.EuclideanDistanceTo(center); // distance of half a pixel to its center.
  std::srand(time(NULL));
  double sum = 0.0;
  while (!iter.IsAtEnd()) {
    typename ImageType::IndexType index = iter.GetIndex();
    typename ImageType::PointType p;
    kernel->TransformIndexToPhysicalPoint(index, p);
    double d = p.EuclideanDistanceTo(center) + h;
    if (d<radius) { // inside the sphere
      iter.Set(1.0);
      sum += 1.0;
    }
    else { // the box intersect the sphere. We randomly pick point in
           // the box and compute the probability to be in/out the
           // sphere
      int n = 500; // number of samples
      double w = 0.0;
      for(auto i=0; i<n; i++) {
        // random position inside the current pixel
        typename ImageType::PointType pos;
        pos[0] = p[0]+(((double)std::rand()/(double)RAND_MAX)-0.5)*spacing[0];
        pos[1] = p[1]+(((double)std::rand()/(double)RAND_MAX)-0.5)*spacing[1];
        pos[2] = p[2]+(((double)std::rand()/(double)RAND_MAX)-0.5)*spacing[2];
        // distance to center
        double distance = pos.EuclideanDistanceTo(center);
        // lower/greater than radius
        if (distance < radius) w += 1.0;
      }
      w = w/(double)n;
      iter.Set(w);
      sum += w;
    }
    ++iter;
  }

  // Normalize
  iter.GoToBegin();
  while (!iter.IsAtEnd()) {
    iter.Set(iter.Get()/sum);
    ++iter;
  }

  // Put in cache
  cache[radius] = kernel;

  return kernel;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
MeanFilterImage(const ImageType * input, double radius)
{
  typename ImageType::Pointer kernel = syd::ComputeMeanFilterKernel<ImageType>(input->GetSpacing(), radius);
  //  syd::WriteImage<ImageType>(kernel, "kernel.mhd");

  // Perform the convolution
  typedef itk::ConvolutionImageFilter<ImageType> FilterType;
  auto filter = FilterType::New();
  filter->SetInput(input);
  filter->SetKernelImage(kernel);
  filter->Update();
  return filter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType, class MaskImageType>
typename ImageType::PointType
GetMaxPosition(const ImageType * input, const MaskImageType * mask)
{
  typedef itk::ImageRegionConstIteratorWithIndex<ImageType> IteratorType;
  typedef itk::ImageRegionConstIteratorWithIndex<MaskImageType> MIteratorType;
  IteratorType iters(input, input->GetLargestPossibleRegion());
  MIteratorType iterm(mask, mask->GetLargestPossibleRegion());
  iters.GoToBegin();
  iterm.GoToBegin();
  double max = 0.0;
  typename ImageType::IndexType index;
  while (!iters.IsAtEnd()) {
    if (iterm.Get() == 1) { // inside the mask
      if (iters.Get() > max) {
        max = iters.Get();
        index = iters.GetIndex();
      }
    }
    ++iters;
    ++iterm;
  }
  typename ImageType::PointType p;
  input->TransformIndexToPhysicalPoint(index, p);
  return p;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename itk::Image<float, ImageType::ImageDimension>::Pointer
CastImageToFloat(ImageType * input)
{
  typedef itk::Image<float, ImageType::ImageDimension> FloatImageType;
  typedef itk::CastImageFilter<ImageType, FloatImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(input);
  filter->Update();
  return filter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void ScaleImage(ImageType * input, double scale)
{
  itk::ImageRegionIterator<ImageType> iter(input, input->GetLargestPossibleRegion());
  while (!iter.IsAtEnd()) {
    iter.Set(iter.Get()*scale);
    ++iter;
  }
}
//--------------------------------------------------------------------


// --------------------------------------------------------------------
template<class PixelType>
typename itk::Image<PixelType,3>::Pointer
ReadDicomFromSingleFile(std::string filename)
{
  typedef itk::Image<PixelType,3> ImageType;
  // Read the image data
  LOG(2) << "Converting dicom file (" << filename << ") to itk image.";
  typename ImageType::Pointer output = ReadImage<ImageType>(filename);

  // Update the iamge
  UpdateImageInformation<PixelType>(output, filename);
  return output;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
typename itk::Image<PixelType,3>::Pointer
ReadDicomSerieFromFolder(std::string folder, std::string serie_uid)
{
  typedef itk::Image<PixelType,3> ImageType;
  typename ImageType::Pointer output;
  std::string file;
  // Read itk image
  try {
    typedef itk::ImageSeriesReader<ImageType> ReaderType;
    typedef itk::GDCMImageIO ImageIOType;
    typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
    InputNamesGeneratorType::Pointer nameGenerator = InputNamesGeneratorType::New();
    nameGenerator->SetInputDirectory(folder);
    const std::vector<std::string> & temp = nameGenerator->GetFileNames(serie_uid);
    typename ReaderType::Pointer reader = ReaderType::New();
    LOG(2) << "Loading " << temp.size() << " files for serie " << serie_uid << " in " << folder << ".";
    reader->SetFileNames(temp);
    file = temp[0];
    reader->Update();
    output = reader->GetOutput();
  }
  catch (itk::ExceptionObject &excp) {
   EXCEPTION("Error while reading the dicom serie in " << folder << ", itk exception is: " << excp);
  }

  // Update the image
  UpdateImageInformation<PixelType>(output, file);
  return output;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
void UpdateImageInformation(typename itk::Image<PixelType,3>::Pointer image, const std::string & filename)
{
  // Open the dicom to read some tags
  typedef itk::Image<PixelType,3> ImageType;
  DcmFileFormat dfile;
  bool b = syd::OpenDicomFile(filename.c_str(), dfile);
  if (!b) {
    EXCEPTION("Could not open the dicom file '" << filename
              << "' maybe this is not a dicom ?");
  }
  DcmObject *dset = dfile.getDataset();

  // Remove meta information (if not : garbage in the mhd)
  itk::MetaDataDictionary d;
  image->SetMetaDataDictionary(d);

  // Offset
  std::string ImagePositionPatient = GetTagValueString(dset, "ImagePositionPatient");
  if (ImagePositionPatient == "") {
    EXCEPTION("Error while reading tag ImagePositionPatient in the dicom ");
  }

  int n = ImagePositionPatient.find("\\");
  std::string sx = ImagePositionPatient.substr(0,n);
  ImagePositionPatient = ImagePositionPatient.substr(n+1,ImagePositionPatient.size());
  n = ImagePositionPatient.find("\\");
  std::string sy = ImagePositionPatient.substr(0,n);
  std::string sz = ImagePositionPatient.substr(n+1,ImagePositionPatient.size());
  typename ImageType::PointType origin;
  origin[0] = ToDouble(sx);
  origin[1] = ToDouble(sy);
  origin[2] = ToDouble(sz);
  // Heuristic : sometimes the origin is not correctly set, so we
  // correct. Warning : the third dimension could be wrong (a single
  // dicom file is open)
  if (image->GetOrigin()[0] != origin[0]) {
    LOG(2) << "Change image origin from " << image->GetOrigin() << " to " << origin;
    image->SetOrigin(origin);
  }

  // Correct for negative SpacingBetweenSlices
  double s = GetTagValueDouble(dset, "SpacingBetweenSlices");

  // change spacing z
  typename ImageType::SpacingType spacing = image->GetSpacing();
  if (s<0) spacing[2] = -s;
  else spacing[2] = s;
  image->SetSpacing(spacing);
  // Direction
  if (s<0) {
    LOG(2) << "Negative spacing, I flip the image.";
    typename ImageType::DirectionType direction = image->GetDirection();
    direction.Fill(0.0);
    direction(0,0) = 1; direction(1,1) = 1; direction(2,2) = -1;
    image->SetDirection(direction);

    // Change orientation
    typename itk::OrientImageFilter<ImageType,ImageType>::Pointer orienter =
      itk::OrientImageFilter<ImageType,ImageType>::New();
    orienter->UseImageDirectionOn();
    orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
    orienter->SetInput(image);
    orienter->Update();
    image = orienter->GetOutput();
  }

  // Correct for pixel scale --> dont know why it does not work.
  // DcmDictEntry * e = new DcmDictEntry(0x0011, 0x103b, EVR_UN, "PixelScale", 0, DcmVariableVM, NULL, true, NULL);
  // DcmDataDictionary &globalDataDict = dcmDataDict.wrlock();
  // globalDataDict.addEntry(e);
  // DcmElement * ee = GetTagValue(dset, "PixelScale");
  // double f;
  // ee->getFloat64(f);
  // DD(f);
  /*
  double scale = GetTagValueDouble(dset, "PixelScale");
  DD(scale);
  if (scale != 1.0 and scale != 0.0) {
    typedef itk::MultiplyImageFilter<ImageType, ImageType, ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput(image);
    filter->SetConstant(scale);
    filter->Update();
    image = filter->GetOutput();
  }
  */
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// template<class PixelType>
// typename itk::Image<PixelType,3>::Pointer CropImageLike(typename itk::Image<PixelType,3>::Pointer image,
//                                                typename itk::Image<PixelType,3>::Pointer like,
//                                                const PixelType & bg) // background value in like
// {
//   DD("CropImageLike");
//   typedef itk::Image<PixelType,3> ImageType;
//   DD(image->GetLargestPossibleRegion());
//   DD(like->GetLargestPossibleRegion());

//   // Create output image




//   ImageType::Pointer output = ImageType::New();


//   /*

//   // Check spacing
//   for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
//     if (like->GetSpacing[i] != input->GetSpacing()[i]) {
//       LOG(FATAL) << "In CropImageLike, images must have the same spacing, but input's spacing is "
//                  << image->GetSpacing() << " while image-like's spacing is " << like->GetSpacing() << ".";
//     }
//   }

//   // Create output image
//   ImageType::Pointer output = ImageType::New();

//   // Set size
//   output->CopyInformation(like);
//   output->SetRegions(like->GetLargestPossibleRegion());
//   output->SetSpacing(like->GetSpacing());
//   output->SetOrigin(like->GetOrigin());
//   output->Allocate();
//   DD(output->GetLargestPossibleRegion());

//   //  output->FillBuffer(GetBackgroundValue());
//  // get startpoint source/dest
//   // for each dim
//   // if source < dest -> start from dest, compute in source
//   // if source > dest -> start from source, compute in dest
//   m_StartDestIndex = output->GetLargestPossibleRegion().GetIndex();
//   m_StartSourceIndex = input->GetLargestPossibleRegion().GetIndex();
//   PointType m_StartPointInSource;
//   PointType m_StartPointInDest;
//   m_StartSourceIndex = input->GetLargestPossibleRegion().GetIndex();
//   input->TransformIndexToPhysicalPoint(m_StartSourceIndex, m_StartPointInSource);
//   m_StartDestIndex = output->GetLargestPossibleRegion().GetIndex();
//   output->TransformIndexToPhysicalPoint(m_StartDestIndex, m_StartPointInDest);
//   IndexType startDestInSource;
//   IndexType startSourceInDest;
//   input->TransformPhysicalPointToIndex(m_StartPointInDest, startDestInSource);
//   output->TransformPhysicalPointToIndex(m_StartPointInSource, startSourceInDest);
//   for(int i=0; i<ImageType::ImageDimension; i++) {
//     if (m_StartPointInSource[i] < m_StartPointInDest[i]) {
//       m_StartSourceIndex[i] = startDestInSource[i];
//     }
//     else {
//       m_StartDestIndex[i] = startSourceInDest[i];
//     }
//   }
//   m_Region.SetIndex(m_StartSourceIndex);

//   // Stop index
//   m_StopSourceIndex = input->GetLargestPossibleRegion().GetIndex()+
//     input->GetLargestPossibleRegion().GetSize();
//   m_StopDestIndex = output->GetLargestPossibleRegion().GetIndex()+
//     output->GetLargestPossibleRegion().GetSize();
//   PointType m_StopPointInSource;
//   PointType m_StopPointInDest;
//   input->TransformIndexToPhysicalPoint(m_StopSourceIndex, m_StopPointInSource);
//   output->TransformIndexToPhysicalPoint(m_StopDestIndex, m_StopPointInDest);
//   IndexType stopDestInSource;
//   IndexType stopSourceInDest;
//   input->TransformPhysicalPointToIndex(m_StopPointInDest, stopDestInSource);
//   output->TransformPhysicalPointToIndex(m_StopPointInSource, stopSourceInDest);

//   for(int i=0; i<ImageType::ImageDimension; i++) {
//     if (m_StopPointInSource[i] > m_StopPointInDest[i]) {
//       m_StopSourceIndex[i] = stopDestInSource[i];
//     }
//     else {
//       m_StopDestIndex[i] = stopSourceInDest[i];
//     }
//   }

//   // Set size to the region we want to paste
//   SizeType s;
//   for(int i=0; i<ImageType::ImageDimension; i++)
//     s[i] = m_StopSourceIndex[i]-m_StartSourceIndex[i];
//   m_Region.SetSize(s);



//   // Paste image inside
//   typedef clitk::PasteImageFilter<ImageType,ImageType> PasteFilterType;
//   typename PasteFilterType::Pointer pasteFilter = PasteFilterType::New();
//   //pasteFilter->ReleaseDataFlagOn(); // change nothing ?
//   //  pasteFilter->InPlaceOn(); // makt it seg fault
//   pasteFilter->SetSourceImage(image);
//   pasteFilter->SetDestinationImage(output);
//   pasteFilter->SetDestinationIndex(m_StartDestIndex);
//   pasteFilter->SetSourceRegion(m_Region);
//   pasteFilter->Update();

//   // Return image
//   return pasteFilter->GetOutput();
//   */
// }
//--------------------------------------------------------------------
