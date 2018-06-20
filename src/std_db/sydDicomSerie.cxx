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
#include "sydStandardDatabase.h" // needed for type odb db
#include "sydTagHelper.h"

DEFINE_TABLE_IMPL(DicomSerie);

// --------------------------------------------------------------------
syd::DicomSerie::DicomSerie():
  syd::Record(),
  syd::DicomBase()
{
  dicom_acquisition_date = empty_value;
  dicom_reconstruction_date = empty_value;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomSerie::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << syd::DicomBase::ToString() << " "
     << dicom_reconstruction_date << " "
     << dicom_acquisition_date;
  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomSerie::AllFieldsToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << syd::DicomBase::AllFieldsToString() << " "
     << dicom_reconstruction_date << " "
     << syd::ArrayToString(dicom_size) << " "
     << syd::ArrayToString(dicom_spacing) << " "
     << dicom_pixel_scale << " "
     << dicom_pixel_offset << " "
     << dicom_real_world_value_slope << " "
     << dicom_real_world_value_intercept << " "
     << dicom_window_center << " "
     << dicom_window_width << " "
     << dicom_radionuclide_name << " "
     << dicom_counts_accumulated << " "
     << dicom_actual_frame_duration_in_msec << " "
     << dicom_energy_window_name << " "
     << dicom_number_of_frames_in_rotation << " "
     << dicom_number_of_rotations << " "
     << dicom_table_traverse_in_mm << " "
     << dicom_table_height_in_mm << " "
     << dicom_rotation_angle << " "
     << dicom_acquisition_date;

  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomSerie::ComputeRelativeFolder() const
{
  if (patient == NULL) {
    LOG(FATAL) << "Error calling ComputeRelativeFolder for a serie, patient pointer is not set. "
               << ToString();
  }
  // Part 1: patient
  std::string f = patient->ComputeRelativeFolder()+PATH_SEPARATOR;

  // Part 2: date
  std::string d = dicom_acquisition_date;
  //  syd::Replace(d, " ", "_");
  // remove the hour and keep y m d
  d = d.substr(0, 10);
  f = f+PATH_SEPARATOR+d;

  // Part 3: modality
  f = f+PATH_SEPARATOR+dicom_modality;
  return f;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomSerie::Callback(odb::callback_event event,
                               odb::database & db) const
{
  syd::Record::Callback(event,db);
  syd::DicomBase::Callback(event, db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomSerie::Callback(odb::callback_event event,
                               odb::database & db)
{
  syd::Record::Callback(event,db);
  syd::DicomBase::Callback(event, db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::DicomSerie::Check() const
{
  return syd::DicomBase::Check();
}
// --------------------------------------------------------------------

// --------------------------------------------------
int syd::DicomSerie::GetYear()
{
  return(std::stoi(dicom_acquisition_date.substr(0,4)));
}
// --------------------------------------------------


// --------------------------------------------------
int syd::DicomSerie::GetMonth()
{
  return(std::stoi(dicom_acquisition_date.substr(5,2)));
}
// --------------------------------------------------


// --------------------------------------------------
int syd::DicomSerie::GetDay()
{
  return(std::stoi(dicom_acquisition_date.substr(8,2)));
}
// --------------------------------------------------


// --------------------------------------------------
int syd::DicomSerie::GetHour()
{
  return(std::stoi(dicom_acquisition_date.substr(11,2)));
}
// --------------------------------------------------


// --------------------------------------------------
int syd::DicomSerie::GetMinute()
{
  return(std::stoi(dicom_acquisition_date.substr(14,2)));
}
// --------------------------------------------------
