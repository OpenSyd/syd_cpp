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

#ifndef SYDDICOMSTRUCTHELPER_H
#define SYDDICOMSTRUCTHELPER_H

// syd
#include "sydDicomStruct.h"
#include "sydDicomSerie.h"
#include "sydRoiMaskImage.h"

// itk
#include <itkImageIOBase.h>

// --------------------------------------------------------------------
namespace syd {

  /// Search for the dicom associated with the struct
  syd::DicomSerie::pointer FindAssociatedDicomSerie(syd::DicomStruct::pointer s);

  /// Create and insert a mask from a dicom struct
  syd::RoiMaskImage::pointer InsertRoiMaskImageFromDicomStruct(syd::DicomStruct::pointer dicom_struct,
                                                               itk::ImageIOBase * header,
                                                               int roi_id,
                                                               syd::RoiType::pointer roi_type,
                                                               bool crop);

  /// Retrieve all Dicom for this patient
  syd::DicomStruct::vector FindDicomStruct(const syd::Patient::pointer patient);

}
// --------------------------------------------------------------------

#endif
