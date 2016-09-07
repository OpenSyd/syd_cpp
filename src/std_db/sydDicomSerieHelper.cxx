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
#include "sydDicomSerieHelper.h"

// std
#include <functional>

// --------------------------------------------------------------------
void syd::WriteDicomToMhd(syd::DicomSerie::pointer dicom,
                          std::string user_pixel_type,
                          std::string mhd_filename)

{
  std::string pixel_type = user_pixel_type;
  if (pixel_type == "auto") {
    // read dicom header with itk header (first file only)
    if (dicom->dicom_files.size() == 0) {
      EXCEPTION("Cannot find associated file to this dicom: "
                << dicom << ". WriteDicomToMhd is aborted.");
    }
    auto header = syd::ReadImageHeader(dicom->dicom_files[0]->GetAbsolutePath());
    pixel_type =
      itk::ImageIOBase::GetComponentTypeAsString(header->GetComponentType());
  }

  // Create a list of pixel_type <-> function
  typedef std::function<void (syd::DicomSerie::pointer, std::string)> FctType;
  std::map<std::string, FctType> map = {
    { "float",  syd::WriteDicomToMhd<itk::Image<float, 3>> },
    { "short",  syd::WriteDicomToMhd<itk::Image<short, 3>> }
  };

  // Looks for the correct pixe_type and use the the corresponding function
  GetFctByPixelType<FctType>(map, pixel_type)(dicom, mhd_filename);
}
// --------------------------------------------------------------------
