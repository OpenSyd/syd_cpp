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
void ConvertDicom_SPECT_ToImage(DcmObject * dset, typename ImageType::Pointer image)
{
  // Check nb of slices
  typename ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
  ushort nbslices = GetTagValueUShort(dset, "NumberOfSlices");
  if (nbslices != size[2]) {
    LOG(FATAL) << "Error image spacing is " << size
               << " while in the dicom NumberOfSlices = " << nbslices;
  }

  // Remove meta information (if not : garbage in the mhd)
  itk::MetaDataDictionary d;
  image->SetMetaDataDictionary(d);

  // Correct for negative SpacingBetweenSlices
  double s = GetTagValueDouble(dset, "SpacingBetweenSlices");
  // change spacing z
  typename ImageType::SpacingType spacing = image->GetSpacing();
  if (s<0) spacing[2] = -s;
  else spacing[2] = s;
  image->SetSpacing(spacing);
  // Direction
  if (s<0) {
    typename ImageType::DirectionType direction = image->GetDirection();
    direction.Fill(0.0);
    direction(0,0) = 1; direction(1,1) = 1; direction(2,2) = -1;
    image->SetDirection(direction);
  }

  // Offset
  std::string ImagePositionPatient = GetTagValueString(dset, "ImagePositionPatient");
  if (ImagePositionPatient == "") {
    LOG(FATAL) << "Error while reading tag ImagePositionPatient in the dicom ";
  }
  int n = ImagePositionPatient.find("\\");
  std::string sx = ImagePositionPatient.substr(0,n);
  ImagePositionPatient = ImagePositionPatient.substr(n+1,ImagePositionPatient.size());
  n = ImagePositionPatient.find("\\");
  std::string sy = ImagePositionPatient.substr(0,n);
  std::string sz = ImagePositionPatient.substr(n+1,ImagePositionPatient.size());
  typename ImageType::PointType origin;
  origin[0] = toDouble(sx);
  origin[1] = toDouble(sy);
  origin[2] = toDouble(sz);
  image->SetOrigin(origin);
}
//--------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ImageType>
void ConvertDicom_CT_ToImage(DcmObject * dset, typename ImageType::Pointer image)
{
  DD("TODO");
  exit(0);
}
//--------------------------------------------------------------------


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
