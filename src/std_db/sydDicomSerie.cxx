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
  injection = NULL;
  size[0] = size[1] = size[2] = 0;
  spacing[0] = spacing[1] = spacing[2] = 0.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomSerie::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << patient->name << " "
     << (injection==NULL ? "no_inj":injection->radionuclide->name) << " "
     << dicom_modality << " "
     << acquisition_date << " "
     << reconstruction_date << " "
     << ArrayToString<int, 3>(size) << " "
     << ArrayToString<double, 3>(spacing) << " "
     << dicom_manufacturer << " "
     << dicom_description;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
bool syd::DicomSerie::IsEqual(const pointer p) const
{
  bool b = (syd::Record::IsEqual(p) and
            patient->IsEqual(p->patient) and
            acquisition_date == p->acquisition_date and
            reconstruction_date == p->reconstruction_date and
            dicom_study_uid == p->dicom_study_uid and
            dicom_series_uid == p->dicom_series_uid and
            dicom_frame_of_reference_uid == p->dicom_frame_of_reference_uid and
            dicom_modality == p->dicom_modality and
            dicom_manufacturer == p->dicom_manufacturer and
            dicom_description == p->dicom_description and
            size == p->size and
            spacing == p->spacing);
  if (!b) return b;
  if (injection == NULL and p->injection == NULL) return b;
  else return injection->IsEqual(p->injection);
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::DicomSerie::ComputeRelativeFolder() const
{
  if (patient == NULL) {
    LOG(FATAL) << "Error calling ComputeRelativeFolder for a serie, patient pointer is not set. " << ToString();
  }
  // Part 1: patient
  std::string f = patient->ComputeRelativeFolder()+PATH_SEPARATOR;

  // Part 2: date
  std::string d = acquisition_date;
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
void syd::DicomSerie::InitTable(syd::PrintTable & ta) const
{
  ta.AddFormat("details", "Print image details");
  ta.AddFormat("file", "Print first dicom filename (slow!)");

  auto & f = ta.GetFormat();

  if (f == "file") {
    ta.AddColumn("id");
    ta.AddColumn("path", 120);
  }
  if (f == "default") {
    ta.AddColumn("id");
    ta.AddColumn("p");
    ta.AddColumn("inj");
    ta.AddColumn("mod");
    ta.AddColumn("acqui_date");
    ta.AddColumn("recon_date");
    auto & c = ta.AddColumn("description", 100);
    c.trunc_by_end_flag = true;
  }
  if (f == "details") {
    ta.AddColumn("id");
    ta.AddColumn("p");
    ta.AddColumn("inj");
    ta.AddColumn("mod");
    ta.AddColumn("acqui_date");
    ta.AddColumn("recon_date");
    ta.AddColumn("size");
    ta.AddColumn("spacing");
    ta.AddColumn("duration(s)",1);
    ta.AddColumn("scale",3);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::DumpInTable(syd::PrintTable & ta) const
{
  auto & f = ta.GetFormat();

  if (f == "file") {
    ta.Set("id", id);
    //Look for associated file (this is slow !)
    syd::DicomFile::vector dfiles;
    typedef odb::query<syd::DicomFile> QDF;
    QDF q = QDF::dicom_serie == id;
    db_->Query<syd::DicomFile>(dfiles, q);
    if (dfiles.size() >= 1) ta.Set("path", dfiles[0]->file->GetAbsolutePath(db_));
    else ta.Set("path", db_->ConvertToAbsolutePath(dfiles[0]->file->path));
  }

  if (f == "default") {
    ta.Set("id", id);
    ta.Set("p", patient->name);
    if (injection == NULL) ta.Set("inj", "no_inj");
    else ta.Set("inj", injection->radionuclide->name);
    ta.Set("mod", dicom_modality);
    ta.Set("acqui_date", acquisition_date);
    ta.Set("recon_date", reconstruction_date);
    ta.Set("description", std::string(dicom_description+" "+dicom_manufacturer));
  }
  if (f == "details") {
    ta.Set("id", id);
    ta.Set("p", patient->name);
    if (injection == NULL) ta.Set("inj", "no_inj");
    else ta.Set("inj", injection->radionuclide->name);
    ta.Set("mod", dicom_modality);
    ta.Set("acqui_date", acquisition_date);
    ta.Set("recon_date", reconstruction_date);
    ta.Set("size", syd::ArrayToString<int, 3>(size));
    ta.Set("spacing", syd::ArrayToString<double, 3>(spacing));
    ta.Set("duration(s)", duration_sec);
    ta.Set("scale", pixel_scale);
  }
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
