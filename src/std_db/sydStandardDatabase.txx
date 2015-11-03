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
syd::StandardDatabase::ReadImage(const syd::DicomSerie::pointer dicom,
                                 bool flipAxeIfNegativeFlag) const
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
      LOG(4) << "ITK reader (sort the list of files according to dicom position)";
      // Cannot use ReadDicomSerieFromListOfFiles because the files are not sorted according to dicom
      // std::vector<std::string> files;
      // for(auto df:dicom_files) files.push_back(df->file->GetAbsolutePath(this));
      // itk_image = syd::ReadDicomSerieFromListOfFiles<PixelType>(folder, files);
      itk_image = syd::ReadDicomSerieFromFolder<PixelType>(folder, dicom->dicom_series_uid);
    }
  } catch (std::exception & e) {
    EXCEPTION("Error '" << e.what() << "' during ReadImage of dicom: " << dicom);
  }

  // Flip ?
  if (flipAxeIfNegativeFlag) {
    if (itk_image->GetDirection()[0][0] < 0) itk_image = syd::FlipImage<ImageType>(itk_image, 0);
    if (itk_image->GetDirection()[1][1] < 0) itk_image = syd::FlipImage<ImageType>(itk_image, 1);
    if (itk_image->GetDirection()[2][2] < 0) itk_image = syd::FlipImage<ImageType>(itk_image, 2);
  }

  return itk_image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ArgsInfo>
void syd::StandardDatabase::SetImageTagsFromCommandLine(syd::Image::pointer image, ArgsInfo args_info)
{
  // Remove all tags
  if (args_info.remove_all_tag_flag) image->tags.clear();

  // Remove some tags
  if (args_info.remove_tag_given) {
    for(auto i=0; i<args_info.remove_tag_given; i++) {
      std::string tagname = args_info.remove_tag_arg[i];
      syd::Tag::vector tags;
      try {
        FindTags(tags, tagname);
      } catch(std::exception & e) { } // ignore unknown tag
      for(auto t:tags) image->RemoveTag(t);
    }
  }

  // Add tags
  if (args_info.tag_given) {
    for(auto i=0; i<args_info.tag_given; i++) {
      std::string tagname = args_info.tag_arg[i];
      syd::Tag::vector tags;
      try {
        FindTags(tags, tagname);
      } catch(std::exception & e) {
        LOG(WARNING) << "Some tags are ignored. " << e.what();
      }
      for(auto t:tags) image->AddTag(t);
    }
  }
}
// --------------------------------------------------------------------
