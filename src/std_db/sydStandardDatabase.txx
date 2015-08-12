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
template<class PixelType>
typename itk::Image<PixelType,3>::Pointer
syd::StandardDatabase::ReadImage(const syd::DicomSerie::pointer dicom) const
{
  typedef itk::Image<PixelType,3> ImageType;

  // Get the files
  sydLOG(4) << "Get the dicom files";
  syd::DicomFile::vector dicom_files;
  odb::query<syd::DicomFile> q = odb::query<syd::DicomFile>::dicom_serie->id == dicom->id;
  Query(dicom_files, q);
  if (dicom_files.size() == 0) {
    EXCEPTION("Error not DicomFile associated with this DicomSerie: " << dicom);
  }
  std::vector<std::string> dicom_filenames;
  for(auto f:dicom_files) {
    dicom_filenames.push_back(GetAbsolutePath(f));
  }
  sydLOG(4) << "Found " << dicom_files.size();

  typename ImageType::Pointer itk_image;
  try {
    if (dicom_filenames.size() == 1) {
      itk_image = syd::ReadDicomFromSingleFile<PixelType>(dicom_filenames[0]);
    }
    else {
      std::string folder = dicom_filenames[0];
      syd::Replace(folder, syd::GetFilenameFromPath(folder), "");
      sydLOG(4) << "ITK reader";
      itk_image = syd::ReadDicomSerieFromFolder<PixelType>(folder, dicom->dicom_series_uid);
    }
  } catch (std::exception & e) {
    EXCEPTION("Error '" << e.what() << "' during ReadImage of dicom: " << dicom);
  }

  return itk_image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class PixelType>
void syd::StandardDatabase::UpdateImageInfo(syd::Image::pointer image,
                                            typename itk::Image<PixelType,3>::Pointer & itk_image,
                                            bool computeMD5Flag)
{
  // Check dimension
  typedef itk::Image<PixelType,3> ImageType;
  image->dimension = itk_image->GetImageDimension();

  //   type, size, spacing, dimension
  if (typeid(PixelType) == typeid(float)) image->pixel_type = "float";
  else {
    if (typeid(PixelType) == typeid(short)) image->pixel_type = "short";
    else {
      if (typeid(PixelType) == typeid(unsigned char)) image->pixel_type = "uchar";
      else {
        EXCEPTION("Cannot create syd::Image from itk image with pixeltype = " << typeid(PixelType).name());
      }
    }
  }

  // Size
  typedef typename ImageType::RegionType RegionType;
  const RegionType & region = itk_image->GetLargestPossibleRegion();
  image->size[0] = region.GetSize()[0];
  image->size[1] = region.GetSize()[1];
  image->size[2] = region.GetSize()[2];

  // Spacing
  image->spacing[0] = itk_image->GetSpacing()[0];
  image->spacing[1] = itk_image->GetSpacing()[1];
  image->spacing[2] = itk_image->GetSpacing()[2];

  // MD5
  if (computeMD5Flag) {
    std::string md5 = syd::ComputeImageMD5<ImageType>(itk_image);
    if (image->type == "mhd") {
      if (image->files.size() != 2) {
        sydLOG(FATAL) << "Error while updating md5 for the image, I expect 2 Files: " << image;
      }
      image->files[1]->md5 = md5;
      Update(image->files[1]);
    }
  }
}
// --------------------------------------------------------------------
