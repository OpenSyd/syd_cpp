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
