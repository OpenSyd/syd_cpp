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
#include "sydImage.h"

// --------------------------------------------------------------------
void syd::ConvertDicomToImage(std::string dicom_filename, std::string mhd_filename)
{
  // Open the dicom
  DcmFileFormat dfile;
  bool b = syd::OpenDicomFile(dicom_filename.c_str(), true, dfile);
  DcmObject *dset = dfile.getDataset();
  if (!b) {
    LOG(FATAL) << "Could not open the dicom file " << dicom_filename;
  }

  // Check if spect or ct
  std::string modality = GetTagValueString(dset, "Modality");
  if (modality != "CT") modality = "NM";

  // Do it for CT (image is signed short)
  if (modality == "CT") {
    typedef itk::Image<signed short, 3> ImageType;
    ImageType::Pointer image = ReadImage<ImageType>(dicom_filename);
    syd::ConvertDicom_CT_ToImage<ImageType>(dset, image);
    syd::WriteImage<ImageType>(image, mhd_filename);
  }

  // Do it for NM (image is float)
  if (modality != "CT") {
    typedef itk::Image<float, 3> ImageType;
    ImageType::Pointer image = ReadImage<ImageType>(dicom_filename);
    syd::ConvertDicom_SPECT_ToImage<ImageType>(dset, image);
    syd::WriteImage<ImageType>(image, mhd_filename);
  }
}
//--------------------------------------------------------------------
