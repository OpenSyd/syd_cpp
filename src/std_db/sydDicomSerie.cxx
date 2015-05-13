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
syd::DicomSerie::~DicomSerie()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomSerie::DicomSerie(const syd::DicomSerie & other) { copy(other); }
// --------------------------------------------------


// --------------------------------------------------
syd::DicomSerie & syd::DicomSerie::operator= (const syd::DicomSerie & other) {
  if (this != &other) { copy(other); }
  return *this;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::copy(const syd::DicomSerie & t) {
  id = t.id;
  patient = t.patient;
  injection = t.injection;

  dicom_study_uid = t.dicom_study_uid;
  dicom_series_uid = t.dicom_series_uid;
  //  dicom_sop_uid = t.dicom_sop_uid;
  //  dicom_dataset_uid = t.dicom_dataset_uid;
  //  dicom_instance_number = t.dicom_instance_number;
  dicom_frame_of_reference_uid = t.dicom_frame_of_reference_uid;

  acquisition_date = t.acquisition_date;
  reconstruction_date = t.reconstruction_date;

  dicom_modality = t.dicom_modality;
  dicom_manufacturer = t.dicom_manufacturer;
  dicom_description = t.dicom_description;

  //  filenames = t.filenames;
  size = t.size;
  spacing = t.spacing;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::SetValues(std::vector<std::string> & arg)
{
  LOG(FATAL) << "Could not insert a DicomSerie with this function. Try sydInsertDicom.";
}
// --------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomSerie::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << patient->name << " "
     << (injection==NULL ? "-":injection->radionuclide) << " "
     << dicom_modality << " "
     << acquisition_date << " "
     << reconstruction_date << " "
     << ArrayToString<int, 3>(size) << " "
     << dicom_manufacturer << " "
     << dicom_description;
  return ss.str();
}
// --------------------------------------------------------------------
