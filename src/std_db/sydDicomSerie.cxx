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
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
syd::DicomSerie::DicomSerie():syd::Record()
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
     << dicom_modality << " "
     << dicom_acquisition_date << " "
     << dicom_reconstruction_date << " "
     << dicom_description << " "
     << dicom_frame_of_reference_uid << " "
     << dicom_study_uid << " "
     << dicom_series_uid;
  return ss.str();
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
void syd::DicomSerie::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event,db);
  // Special case: when a serie is deleted, the linked DicomFile will
  // be deleted by the oncascade, but the callback is not called to
  // erase the file, so we do it here.
  if (event == odb::callback_event::pre_erase) {
    syd::DicomFile::vector dfiles;
    typedef odb::result<syd::DicomFile> result;
    odb::query<syd::DicomFile> q = odb::query<syd::DicomFile>::dicom_serie == id;
    result r(db.query<DicomFile>(q));
    for(auto i = r.begin(); i != r.end(); i++) {
      syd::DicomFile::pointer s = syd::DicomFile::New();
      i.load(*s);
      dfiles.push_back(s);
    }
    for(auto d:dfiles) d->Callback(event, db);
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
  ta.Set("acqui_date", dicom_acquisition_date);
  ta.Set("mod", dicom_modality);
  ta.Set("recon_date", dicom_reconstruction_date);
  ta.Set("description", dicom_description, 100);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::DumpInTable_file(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", patient->name);
  ta.Set("mod", dicom_modality);
  ta.Set("acqui_date", dicom_acquisition_date);

  //Look for associated file (this is slow !)
  syd::DicomFile::vector dfiles;
  typedef odb::query<syd::DicomFile> QDF;
  QDF q = QDF::dicom_serie == id;
  db_->Query<syd::DicomFile>(dfiles, q);
  if (dfiles.size() >= 1) ta.Set("path", dfiles[0]->file->GetAbsolutePath(), 150);
  else ta.Set("path", db_->ConvertToAbsolutePath(dfiles[0]->file->path), 150);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::DumpInTable_filelist(syd::PrintTable & ta) const
{
  ta.SetSingleRowFlag(true);
  ta.SetHeaderFlag(false);
  //Look for associated file (this is slow !)
  syd::DicomFile::vector dfiles;
  typedef odb::query<syd::DicomFile> QDF;
  QDF q = QDF::dicom_serie == id;
  db_->Query<syd::DicomFile>(dfiles, q);
  if (dfiles.size() >= 1) // FIXME -> build a string with all filenames (?)
    ta.Set("file", dfiles[0]->file->GetAbsolutePath(), 500);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::DumpInTable_details(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", patient->name);
  ta.Set("description", dicom_description, 200);
}
// --------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::DicomSerie::Check() const
{
  syd::CheckResult r;

  //Look for associated file (this is slow !)
  syd::DicomFile::vector dfiles;
  typedef odb::query<syd::DicomFile> QDF;
  QDF q = QDF::dicom_serie == id;
  db_->Query<syd::DicomFile>(dfiles, q);
  for(auto d:dfiles) r.merge(d->Check());

  return r;
}
// --------------------------------------------------------------------
