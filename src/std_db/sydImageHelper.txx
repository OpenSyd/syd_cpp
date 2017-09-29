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
template<class ArgsInfo>
void syd::SetImageInfoFromCommandLine(syd::Image::pointer image,
                                      ArgsInfo & args_info)
{
  auto db = image->GetDatabase<syd::StandardDatabase>();

  if (args_info.pixel_unit_given)
    syd::SetPixelUnit(image, args_info.pixel_unit_arg);

  if (args_info.modality_given) image->modality = args_info.modality_arg;

  if (args_info.frame_of_reference_uid_given)
    image->frame_of_reference_uid = args_info.frame_of_reference_uid_arg;

  if (args_info.acquisition_date_given) {
    auto d = args_info.acquisition_date_arg;
    if (!syd::IsDateValid(d)) {
      LOG(FATAL) << "Acquisition date is not valid.";
    }
    image->acquisition_date = d;
  }

  if (args_info.patient_given) {
    auto patient = db->FindPatient(args_info.patient_arg);
    image->patient = patient;
  }

  if (args_info.injection_given)
    syd::SetInjection(image, args_info.injection_arg);

  if (args_info.dicom_given) {
    for(auto i=0; i<args_info.dicom_given; i++)
      syd::AddDicomSerie(image, args_info.dicom_arg[i]);
  }

  if (args_info.flip_if_negative_spacing_flag and
      syd::FlipImageIfNegativeSpacing(image)) {
    LOG(1) << "Negative spacing detected, image was flipped";
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<typename ImageType>
syd::Image::pointer syd::InsertImage(typename ImageType::Pointer itk_image,
                                     syd::Patient::pointer patient,
                                     std::string modality,
                                     int dimension)
{
  // New image
  auto db = patient->GetDatabase();
  auto image = db->New<syd::Image>();
  image->patient = patient;
  image->modality = modality;
  image->type = "mhd";
  db->Insert(image);

  // save the image
  std::string filename = db->GetUniqueTempFilename();
  syd::WriteImage<ImageType>(itk_image, filename, dimension);

  // insert the files
  image->files = syd::InsertFilesFromMhd(db, filename,
                                         image->ComputeDefaultRelativePath(),
                                         image->ComputeDefaultMHDFilename());
  syd::DeleteMHDImage(filename);

  // Update size and spacing
  syd::SetImageInfoFromFile(image);
  db->Update(image);
  return image;
}
// --------------------------------------------------------------------
