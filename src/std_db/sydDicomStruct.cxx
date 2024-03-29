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
#include "sydDicomStruct.h"
#include "sydStandardDatabase.h" // needed for type odb db
#include "sydTagHelper.h"

DEFINE_TABLE_IMPL(DicomStruct);

// --------------------------------------------------------------------
syd::DicomStruct::DicomStruct():
  syd::Record(),
  syd::DicomBase()
{
  dicom_structure_set_date = empty_value;
  dicom_station_name = empty_value;
  dicom_protocol_name = empty_value;
  dicom_structure_set_label = empty_value;
  dicom_structure_set_name = empty_value;
  dicom_structure_set_date = empty_value;
  associatedImage = nullptr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomStruct::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << syd::DicomBase::ToString()
     << dicom_structure_set_date << " "
     << dicom_structure_set_name << " "
     << dicom_structure_set_label << " "
     << dicom_roi_names.size();
  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomStruct::AllFieldsToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << syd::DicomBase::AllFieldsToString() << " "
     << dicom_structure_set_date << " "
     << dicom_structure_set_name << " "
     << dicom_structure_set_label << " "
     << dicom_roi_names.size() << " "
     << dicom_station_name << " "
     << dicom_protocol_name << " ";
  if (associatedImage != nullptr)
    ss << associatedImage->id << " ";
  else
    ss << "- ";
  ss << syd::ArrayToString(dicom_roi_names);

  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomStruct::ComputeRelativeFolder() const
{
  if (patient == NULL) {
    LOG(FATAL) << "Error calling ComputeRelativeFolder for a serie, patient pointer is not set. "
               << ToString();
  }
  // Folder name: patient + 'roi'
  std::string f = patient->ComputeRelativeFolder()+PATH_SEPARATOR+"roi"+PATH_SEPARATOR;

  return f;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomStruct::Callback(odb::callback_event event,
                                odb::database & db) const
{
  syd::Record::Callback(event,db);
  syd::DicomBase::Callback(event, db);

  if (event == odb::callback_event::pre_persist or
      event == odb::callback_event::pre_update) CheckAndCorrectROI();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomStruct::Callback(odb::callback_event event,
                                odb::database & db)
{
  syd::Record::Callback(event,db);
  syd::DicomBase::Callback(event, db);

  if (event == odb::callback_event::pre_persist or
      event == odb::callback_event::pre_update) CheckAndCorrectROI();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::DicomStruct::Check() const
{
  return syd::DicomBase::Check();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomStruct::CheckAndCorrectROI() const
{
  if (dicom_roi_names.size() == roi_types.size()) return;
  roi_types.resize(dicom_roi_names.size()); // vector of nullptr
}
// --------------------------------------------------------------------
