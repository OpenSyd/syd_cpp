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
void syd::ImageFromDicomBuilder::WriteMhd(std::string temp_filename)
{
  if (dicom_files_.size() == 0) {
    EXCEPTION("No associated DicomFiles in UpdateWithPixelType. Use Update() before.");
  }
  typedef itk::Image<PixelType, 3> ImageType;
  typename ImageType::Pointer itk_image;

  // Try to read the dicom image
  auto df = dicom_files_[0];
  try {
    if (dicom_files_.size() == 1) {
      itk_image = syd::ReadDicomFromSingleFile<PixelType>(df->GetAbsolutePath());
    }
    else {
      std::string folder = df->file->path;
      folder = dicom_->GetDatabase()->ConvertToAbsolutePath(folder);
      // Cannot use ReadDicomSerieFromListOfFiles because the files
      // are not sorted according to dicom
      itk_image = syd::ReadDicomSerieFromFolder<PixelType>(folder, dicom_->dicom_series_uid);
    }
  } catch (std::exception & e) {
    EXCEPTION("Error '" << e.what() << "' during ReadImage of dicom: " << dicom_);
  }

  // write mhd
  syd::WriteImage<ImageType>(itk_image, temp_filename);
}
// --------------------------------------------------------------------
