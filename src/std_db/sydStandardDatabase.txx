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
syd::StandardDatabase::ReadImage(const syd::DicomSerie::pointer dicom)
{
  typedef itk::Image<PixelType,3> ImageType;

  // Get the files
  LOG(4) << "Get the dicom files";
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
  LOG(4) << "Found " << dicom_files.size();

  typename ImageType::Pointer itk_image;
  try {
    if (dicom_filenames.size() == 1) {
      itk_image = syd::ReadDicomFromSingleFile<PixelType>(dicom_filenames[0]);
    }
    else {
      std::string folder = dicom_filenames[0];
      syd::Replace(folder, syd::GetFilenameFromPath(folder), "");
      LOG(4) << "ITK reader";
      itk_image = syd::ReadDicomSerieFromFolder<PixelType>(folder, dicom->dicom_series_uid);
    }
  } catch (std::exception & e) {
    EXCEPTION("Error '" << e.what() << "' during ReadImage of dicom: " << dicom);
  }

  return itk_image;
}
// --------------------------------------------------------------------
