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
  syd::RecordWithComments(),
  syd::RecordWithTags()
{
  patient = NULL;
  dicom_modality = dicom_acquisition_date
    = dicom_reconstruction_date = dicom_description
    = dicom_frame_of_reference_uid = empty_value;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomSerie::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << (patient != NULL? patient->name:empty_value) << " "
     << dicom_files.size() << " "
     << dicom_modality << " "
     << dicom_acquisition_date << " "
     << dicom_reconstruction_date << " "
     << dicom_description << " "
     << dicom_frame_of_reference_uid << " "
     << dicom_study_uid << " "
     << dicom_series_uid << " "
     << syd::GetLabels(tags) << " "
     << GetAllComments();
  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------
std::string syd::DicomSerie::ComputeRelativeFolder() const
{
  if (patient == NULL) {
    LOG(FATAL) << "Error calling ComputeRelativeFolder for a serie, patient pointer is not set. " << ToString();
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
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::Callback(odb::callback_event event,
                               odb::database & db) const
{
  syd::Record::Callback(event,db);

  // not needed, but safer (be sure to store file modif)
  if (event == odb::callback_event::pre_update) {
    // update the files
    for(auto f:dicom_files) db.update(f);
  }

  // When a serie is deleted, we need to delete the
  // associated DicomFiles
  if (event == odb::callback_event::pre_erase) {
    for(auto d:dicom_files) db.erase(d); //
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event,db);
}
// --------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::DicomSerie::Check() const
{
  syd::CheckResult r;
  for(auto d:dicom_files) r.merge(d->Check());
  return r;
}
// --------------------------------------------------------------------
