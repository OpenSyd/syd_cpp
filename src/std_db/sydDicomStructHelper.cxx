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

// --------------------------------------------------------------------
syd::DicomSerie::pointer syd::FindAssociatedDicomSerie(syd::DicomStruct::pointer dicom_struct)
{
  DDF();

  auto filename = dicom_struct->dicom_files[0]->GetAbsolutePath();
  DD(filename);

  auto reader = syd::ReadDicomStructHeader(filename); // will raise exception if not ok
  auto dataset = reader.GetFile().GetDataSet();

  DD("here");

  gdcm::Tag tsssq(0x3006,0x0014);
  if (!dataset.FindDataElement(tsssq)) {
    LOG(FATAL) << "Problem locating XXXX?" << std::endl; /// FIXME bug here
  }
  DD("here");
  auto & sssq = dataset.GetDataElement(tsssq);
  DD("here");
  auto seq = sssq.GetValueAsSQ();
  DD("here");
  // for(auto i = 0; i < seq->GetNumberOfItems(); ++i){
  //   DD(i);
  auto & item = seq->GetItem(0); // Item starts at 1
  DD("here");
  auto & nested_dataset = item.GetNestedDataSet();
  DD("here");
  auto s = syd::GetTagValueAsString<0x20,0x0e>(nested_dataset);
  DD(s);
  // }

  // std::string s =
  //   syd::GetTagValueAsString<0x20,0x0e>(dataset);
  // DD(s);


}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer syd::InsertRoiMaskImageFromDicomStruct(syd::DicomStruct::pointer dicom_struct,
                                                                  itk::ImageIOBase * header,
                                                                  int roi_id,
                                                                  syd::RoiType::pointer roi_type)
{
  DDF();
  DD(dicom_struct);
  DD(roi_type);
  DD(roi_id);

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

  // Create the RoiMaskImage
  auto db = dicom_struct->GetDatabase<syd::StandardDatabase>();
  filename = db->GetUniqueTempFilename(".mhd");
  syd::WriteImage<ImageType>(image, filename);
  DD(filename);
  auto mask = syd::InsertRoiMaskImageFromFile(filename, dicom_struct->patient, roi_type);
  mask->frame_of_reference_uid = dicom_struct->dicom_frame_of_reference_uid;
  mask->acquisition_date = dicom_struct->dicom_structure_set_date;
  DD(mask);
  fs::remove(filename);

  return mask;
}
// --------------------------------------------------------------------
