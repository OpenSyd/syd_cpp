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

#ifndef SERIE_H
#define SERIE_H

#include <string>
#include <iostream>
#include <odb/core.hxx>

typedef unsigned int IdType;

// --------------------------------------------------------------------
#pragma db object
class Serie
{
public:

#pragma db id auto
  IdType        id;
  IdType        patient_id;
  std::string   dicom_uid;
  std::string   dicom_dataset_name;
  std::string   dicom_image_id;
  std::string   dicom_study_desc;
  std::string   dicom_series_desc;
  std::string   dicom_frame_of_reference_uid;
  std::string   dicom_manufacturer;
  std::string   dicom_manufacturer_model_name;
  std::string   dicom_instance_number;
  int           number_of_files;
  std::string   path;
  std::string   acquisition_date;
  std::string   reconstruction_date;
  std::string   modality;

  friend std::ostream& operator<<(std::ostream& os, const Serie & p) {
    os << p.id << " " << p.acquisition_date << " " << p.dicom_series_desc << " " << p.dicom_image_id;
    return os;
  }

};
// --------------------------------------------------------------------

#endif
