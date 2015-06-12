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

#ifndef SYDVIEW_H
#define SYDVIEW_H

// syd
#include "sydDicomSerie.h"
#include "sydImage.h"

// --------------------------------------------------------------------
namespace syd {

// View (query)
#pragma db view                                                         \
  table("Image")                                                        \
  table("Image_dicoms": "Image.id = Image_dicoms.object_id ")           \
  table("DicomSerie" : "DicomSerie.id = Image_dicoms.value")

// query("SELECT object_id, value "                \
//                       "FROM Image_dicoms,Image,DicomSerie"      \
//                       "WHERE Image.id ==173;")
//                       // "WHERE Image_dicoms.object_id==image.id and value == DicomSerie.id and " \
                      // "dicom_description LIKE (?)"  )
  struct Image_Dicoms_View
  {
#pragma db column("Image_Dicoms.object_id")
    IdType image_id;

#pragma db column("Image_Dicoms.value")
    IdType dicom_id;

#pragma db column("DicomSerie.dicom_modality")
    std::string dicom_modality;

#pragma db column("DicomSerie.dicom_description")
    std::string dicom_description;

// #pragma db column("DicomSerie.acquisition_date")
//     std::string acquisition_date;

  };


} // end namespace
// --------------------------------------------------------------------

#endif
