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
void syd::WriteDicomToMhd(syd::DicomSerie::pointer dicom,
                          std::string mhd_filename)
{
  // consider the associated dicom files
  auto dicom_files = dicom->dicom_files;
  if (dicom_files.size() == 0) {
    EXCEPTION("No associated DicomFiles in this DicomSerie. Cannot convert to mhd.");
  }

  // Try to read the dicom image
  auto df = dicom_files[0];
  typedef itk::Image<PixelType, 3> ImageType;
  typename ImageType::Pointer itk_image;
  try {
    if (dicom_files.size() == 1) {
      itk_image = syd::ReadDicomFromSingleFile<PixelType>(df->GetAbsolutePath());
    }
    else {
      std::string folder = df->path;
      folder = dicom->GetDatabase()->ConvertToAbsolutePath(folder);
      // Cannot use ReadDicomSerieFromListOfFiles because the files
      // are not sorted according to dicom
      itk_image = syd::ReadDicomSerieFromFolder<PixelType>(folder, dicom->dicom_series_uid);
    }
  } catch (std::exception & e) {
    EXCEPTION("Error '" << e.what()
              << "' during ReadImage of dicom: " << dicom);
  }

  // write mhd
  syd::WriteImage<ImageType>(itk_image, mhd_filename);
}
// --------------------------------------------------------------------
