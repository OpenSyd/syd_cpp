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
#include "sydDicomSerie.h"

// --------------------------------------------------------------------
syd::DicomSerie::DicomSerie():TableElement()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomSerie::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << patient->name << " "
     << (injection==NULL ? "-":injection->radionuclide->name) << " "
     << dicom_modality << " "
     << acquisition_date << " "
     << reconstruction_date << " "
     << ArrayToString<int, 3>(size) << " "
     << dicom_manufacturer << " "
     << dicom_description;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
bool syd::DicomSerie::operator==(const DicomSerie & p)
{
  return (id == p.id and
          *patient == *p.patient and
          *injection == *p.injection and
          acquisition_date == p.acquisition_date and
          reconstruction_date == p.reconstruction_date and
          dicom_study_uid == p.dicom_study_uid and
          dicom_series_uid == p.dicom_series_uid and
          dicom_frame_of_reference_uid == p.dicom_frame_of_reference_uid and
          dicom_modality == p.dicom_modality and
          dicom_manufacturer == p.dicom_manufacturer and
          dicom_description == p.dicom_description and
          size == p.size and
          spacing == p.spacing);

}
// --------------------------------------------------