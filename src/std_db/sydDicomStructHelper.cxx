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

// syd
#include "sydDicomStructHelper.h"
#include "sydDicomStructToImage.h"
#include "sydImageUtils.h"
#include "sydRoiMaskImageHelper.h"
#include "sydStandardDatabase.h"
#include "sydImageCrop.h"


// --------------------------------------------------------------------
syd::DicomSerie::pointer syd::FindAssociatedDicomSerie(syd::DicomStruct::pointer dicom_struct)
{
  // Get the dicom file and read diom tags
  auto filename = dicom_struct->dicom_files[0]->GetAbsolutePath();
  auto reader = syd::ReadDicomStructHeader(filename); // will raise exception if not ok
  auto & dataset = reader.GetFile().GetDataSet();

  // Get Referenced Frame of Reference Sequence
  auto seq = GetSequence(dataset, 0x3006,0x0010);
  auto item = seq->GetItem(1);

  // Get RT Referenced Study Sequence
  seq = GetSequence(item.GetNestedDataSet(), 0x3006,0x0012);
  item = seq->GetItem(1);

  // Get RT Referenced Series Sequence
  seq = GetSequence(item.GetNestedDataSet(), 0x3006,0x0014);
  item = seq->GetItem(1);

  // Get Series Instance UID
  auto & nested_dataset = item.GetNestedDataSet();
  auto s = syd::GetTagValueAsString<0x20,0x0e>(nested_dataset);

  // Look for DicomSerie with the same UID
  syd::DicomSerie::vector dicoms;
  typedef odb::query<syd::DicomSerie> QI;
  QI q = QI::patient == dicom_struct->patient->id and QI::dicom_series_uid == s;
  auto db = dicom_struct->GetDatabase();
  db->Query(dicoms, q);

  // There can be only one!
  if (dicoms.size() == 1) return dicoms[0];
  if (dicoms.size() == 0) return nullptr;
  DDS(dicoms);
  EXCEPTION("Error not only one dicom seems associated with this struct " << s);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer syd::InsertRoiMaskImageFromDicomStruct(syd::DicomStruct::pointer dicom_struct,
                                                                  itk::ImageIOBase * header,
                                                                  int roi_id,
                                                                  syd::RoiType::pointer roi_type,
                                                                  bool crop)
{
  // Get dicom dataset
  if (dicom_struct->dicom_files.size() != 1) {
    EXCEPTION("Error while reading datase in DicomStruct file. Expect a single file, found "
              << dicom_struct->dicom_files.size());
  }
  auto filename = dicom_struct->dicom_files[0]->GetAbsolutePath();
  auto reader = syd::ReadDicomStructHeader(filename);
  auto & dataset = reader.GetFile().GetDataSet(); // reader object must not be destroyed to use dataset

  // Create empty 3D image
  typedef syd::DicomStructToImageBuilder::MaskImageType ImageType;
  auto image = syd::CreateImageLike<ImageType>(header);

  // Create the mask
  syd::DicomStructToImageBuilder builder;
  builder.ConvertRoiToImage(dataset, roi_id, image);

  // Crop
  if (crop)
    image = syd::CropImageWithLowerThreshold<ImageType>(image, 1);

  // Create the RoiMaskImage
  auto db = dicom_struct->GetDatabase<syd::StandardDatabase>();
  filename = db->GetUniqueTempFilename(".mhd");
  syd::WriteImage<ImageType>(image, filename);
  auto mask = syd::InsertRoiMaskImageFromFile(filename, dicom_struct->patient, roi_type);
  mask->frame_of_reference_uid = dicom_struct->dicom_frame_of_reference_uid;
  mask->acquisition_date = dicom_struct->dicom_structure_set_date;
  fs::remove(filename);

  return mask;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomStruct::vector syd::FindDicomStruct(const syd::Patient::pointer patient)
{
  auto db = patient->GetDatabase();
  syd::DicomStruct::vector dicoms;
  typedef odb::query<syd::DicomStruct> QI;
  QI q = QI::patient == patient->id;
  db->Query(dicoms, q);
  return dicoms;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::GetRoiIndexFromName(syd::DicomStruct::pointer dicom,
                             std::string dicom_roi_name)
{
  auto filename = dicom->dicom_files[0]->GetAbsolutePath();
  auto reader = syd::ReadDicomStructHeader(filename);
  auto & dataset = reader.GetFile().GetDataSet(); // reader object must not be destroyed to use dataset

  // Get Structure Set ROI Sequence
  auto roi_seq = syd::GetSequence(dataset, 0x3006,0x0020);
  for(auto i = 0; i < roi_seq->GetNumberOfItems(); ++i){
    auto & item = roi_seq->GetItem(i+1); // Item starts at 1
    auto & nested_dataset = item.GetNestedDataSet();
    auto name = syd::GetTagValueAsString<0x3006,0x26>(nested_dataset);
    if (name == dicom_roi_name) {
      return i+1; // Start at 1 not zero
    }
  }
  return -1;
}
// --------------------------------------------------------------------


