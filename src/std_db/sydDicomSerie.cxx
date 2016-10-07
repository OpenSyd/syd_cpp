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

// --------------------------------------------------------------------
syd::DicomSerie::DicomSerie():
  syd::Record(),
  syd::RecordWithComments()
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


// --------------------------------------------------
void syd::DicomSerie::DumpInTable(syd::PrintTable & ta) const
{
  auto format = ta.GetFormat();
  if (format == "default") DumpInTable_default(ta);
  else if (format == "file") DumpInTable_file(ta);
  else if (format == "filelist") DumpInTable_filelist(ta);
  else if (format == "details") DumpInTable_details(ta);
  {
    ta.AddFormat("default", "id, date, tags, size etc");
    ta.AddFormat("file", "with complete filename");
    ta.AddFormat("filelist", "not a table a list of filenames");
    ta.AddFormat("details", "other informations");
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::DumpInTable_default(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", patient->name);
  ta.Set("files", dicom_files.size());
  ta.Set("acqui_date", dicom_acquisition_date);
  ta.Set("mod", dicom_modality);
  ta.Set("recon_date", dicom_reconstruction_date);
  ta.Set("description", dicom_description, 400);
  ta.Set("com", GetAllComments());
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::DumpInTable_file(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", patient->name);
  ta.Set("mod", dicom_modality);
  ta.Set("acqui_date", dicom_acquisition_date);
  if (dicom_files.size() > 0) ta.Set("path", dicom_files[0]->GetAbsolutePath(), 150);
  else ta.Set("path", empty_value);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::DumpInTable_filelist(syd::PrintTable & ta) const
{
  ta.SetSingleRowFlag(true);
  ta.SetHeaderFlag(false);
  std::stringstream ss;
  for(auto f:dicom_files) ss << f->GetAbsolutePath() << " ";
  ta.Set("file", ss.str(), ss.str().size());
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::DumpInTable_details(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", patient->name);
  ta.Set("frame", dicom_frame_of_reference_uid);
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
