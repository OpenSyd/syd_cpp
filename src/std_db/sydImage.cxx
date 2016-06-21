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
#include "sydImage.h"
#include "sydStandardDatabase.h"
#include "sydDicomSerie.h"
#include "sydImageBuilder.h"
#include "sydTag.h"

// --------------------------------------------------------------------
syd::Image::Image():syd::RecordWithHistory()
{
  type = "unset";
  pixel_type = "unset";
  dimension = 3;
  frame_of_reference_uid = "unset";
  files.clear();
  dicoms.clear();
  for(auto &s:size) s = 0;
  for(auto &s:spacing) s = 1.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::~Image()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::ToString() const
{
  std::string name;
  if (patient == NULL) name = "unset";
  else name = patient->name;
  std::stringstream ss ;
  ss << id << " "
     << name << " "
     << injection->radionuclide->name << " ";
  if (files.size() == 0) ss << "(no files) ";
  //for(auto f:files) ss << f->filename << " "; // only first is usually useful
  else ss << files[0]->filename << " ";
  ss << GetLabels(tags) << " " << type << " " << pixel_type << " "
     << size[0] << "x" << size[1] << "x" << size[2] << " "
     << spacing[0] << "x" << spacing[1] << "x" << spacing[2];
  if (dicoms.size() > 0) ss << " " << dicoms[0]->dicom_modality << " ";
  for(auto d:dicoms) ss << d->id << " ";
  ss << frame_of_reference_uid << " ";
  if (pixel_value_unit != NULL) ss << pixel_value_unit->name;
  else ss << "pixel_value_unit_unset";
  if (history) {
    ss << " " << history->insertion_date << " "
       << history->update_date;
  }
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
std::string syd::Image::GetAcquisitionDate() const
{
  if (dicoms.size() == 0) return "unknown_date";
  else return dicoms[0]->acquisition_date;
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::Image::GetModality() const
{
  if (dicoms.size() == 0) return "unknown_modality";
  else return dicoms[0]->dicom_modality;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::AddDicomSerie(syd::DicomSerie::pointer dicom)
{
  bool found = false;
  int i=0;
  while (i<dicoms.size() and !found) {
    if (dicoms[i]->id == dicom->id) found = true;
    ++i;
  }
  if (!found) dicoms.push_back(dicom);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::RemoveDicomSerie(syd::DicomSerie::pointer dicom)
{
  bool found = false;
  int i=0;
  while (i<dicoms.size() and !found) {
    if (dicoms[i]->id == dicom->id) {
      found = true;
      dicoms.erase(dicoms.begin()+i);
    }
    ++i;
  }
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::Image::ComputeRelativeFolder() const
{
  return patient->name;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event,db);
  syd::RecordWithHistory::Callback(event,db, db_);

  if (event == odb::callback_event::post_erase) {
    for(auto f:files) db.erase(f);
  }
  if (event == odb::callback_event::pre_persist) {
    // insert the file with odb::database not the syd::database
    for(auto f:files) db.persist(f);
  }
  if (event == odb::callback_event::pre_update) {
    // update the file with odb::database not the syd::database
    for(auto f:files) db.update(f);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event,db);
  syd::RecordWithHistory::Callback(event,db, db_);
  if (event == odb::callback_event::post_erase) {
    for(auto f:files) db.erase(f);
  }
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Image::IsSameSizeAndSpacingThan(const syd::Image::pointer image) const
{
  bool b = true;
  for(auto i=0; i<size.size(); i++) {
    b = b and size[i] == image->size[i] and
      spacing[i] == image->spacing[i];
  }
  return b;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::FatalIfNoDicom() const
{
  if (dicoms.size() == 0) {
    LOG(FATAL) << "Error the following image does not have associated DicomSerie."
               << std::endl << *this;
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::CopyDicomSeries(syd::Image::pointer image)
{
  for(auto d:image->dicoms) AddDicomSerie(d);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable(syd::PrintTable2 & ta) const
{
  auto format = ta.GetFormat();
  if (format == "default") DumpInTable_default(ta);
  else if (format == "short") DumpInTable_short(ta);
  else if (format == "ref_frame") DumpInTable_ref_frame(ta);
  else if (format == "history") DumpInTable_history(ta);
  else if (format == "file") DumpInTable_file(ta);
  else if (format == "filelist") DumpInTable_filelist(ta);
  else {
    ta.AddFormat("default", "id, date, tags, size etc");
    ta.AddFormat("short", "no size");
    ta.AddFormat("ref_frame", "with dicom_reference_frame");
    ta.AddFormat("history", "with date inserted/updated");
    ta.AddFormat("file", "with complete filename");
    ta.AddFormat("filelist", "not a table a list of filenames");
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_short(syd::PrintTable2 & ta) const
{
  ta.Set("id", id);
  if (dicoms.size() > 0)
    ta.Set("acqui_date", dicoms[0]->acquisition_date);
  else ta.Set("acqui_date", "no_date");
  ta.Set("tags", GetLabels(tags), 100);
  if (pixel_value_unit != NULL) ta.Set("unit", pixel_value_unit->name);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_default(syd::PrintTable2 & ta) const
{
  DumpInTable_short(ta);
  ta.Set("inj", injection->radionuclide->name);
  ta.Set("size", syd::ArrayToString<int, 3>(size));
  ta.Set("spacing", syd::ArrayToString<double, 3>(spacing,1));
  std::string dicom;
  for(auto d:dicoms) dicom += syd::ToString(d->id)+" ";
  if (dicom.size() != 0) dicom.pop_back(); // remove last space
  else dicom = "no_dicom";
  ta.Set("dicom", dicom);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_ref_frame(syd::PrintTable2 & ta) const
{
  DumpInTable_short(ta);
  ta.Set("ref_frame", frame_of_reference_uid);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_history(syd::PrintTable2 & ta) const
{
  DumpInTable_short(ta);
  syd::RecordWithHistory::DumpInTable(ta);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_file(syd::PrintTable2 & ta) const
{
  DumpInTable_short(ta);
  ta.Set("file", files[0]->GetAbsolutePath(db_), 100);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_filelist(syd::PrintTable2 & ta) const
{
  ta.SetSingleRowFlag(true);
  ta.SetHeaderFlag(false);
  ta.Set("file", files[0]->GetAbsolutePath(db_), 500);
}
// --------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::Image::Check() const
{
  syd::CheckResult r;
  for(auto f:files) r.merge(f->Check());
  for(auto d:dicoms) r.merge(d->Check());
  return r;
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
double syd::Image::GetHoursFromInjection() const
{
  std::string inj_date = injection->date;
  if (dicoms.size() == 0) {
    LOG(FATAL) << "No dicom attached to this image, cannot compute the time from injection date "
               << ToString() << std::endl;
  }
  double time = syd::DateDifferenceInHours(dicoms[0]->acquisition_date, inj_date);
  return time;
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
std::vector<double> & syd::GetTimesFromInjection(syd::StandardDatabase * db,
                                                 const syd::Image::vector images)
{
  std::vector<double> * times = new std::vector<double>;
  syd::Image::vector sorted_images = images;
  db->Sort<syd::Image>(sorted_images);
  syd::Injection::pointer injection = sorted_images[0]->injection;
  std::string starting_date = injection->date;
  for(auto image:sorted_images) {
    double t = syd::DateDifferenceInHours(image->dicoms[0]->acquisition_date, starting_date);
    times->push_back(t);
  }
  return *times;
}
// --------------------------------------------------------------------
