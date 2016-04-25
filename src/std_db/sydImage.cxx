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
    DD("image post erase const");
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
    DD("image post erase");
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
void syd::Image::InitTable(syd::PrintTable & ta) const
{
  // Define the formats
  auto & f = ta.GetFormat();
  ta.AddFormat("file", "Display the filename");
  ta.AddFormat("filelist", "List of files without line break");
  ta.AddFormat("timing", "Display time in hours from injection");

  // Set the columns
  if (f == "default") {
    ta.AddColumn("id");
    ta.AddColumn("p");
    ta.AddColumn("inj");
    ta.AddColumn("acqui_date");
    ta.AddColumn("tags");
    ta.AddColumn("size");
    ta.AddColumn("spacing");
    ta.AddColumn("dicom");
    ta.AddColumn("unit");
    auto & c = ta.AddColumn("ref_frame");
    c.max_width = 20;
    c.trunc_by_end_flag = false;
  }
  if (f == "timing") {
    ta.AddColumn("id");
    ta.AddColumn("p");
    ta.AddColumn("t", 2);
    ta.AddColumn("tags");
  }
  if (f == "history") {
    syd::RecordWithHistory::InitTable(ta);
    ta.AddColumn("p");
    ta.AddColumn("acqui_date");
    ta.AddColumn("tags");
    ta.AddColumn("unit");
    auto & c = ta.AddColumn("ref_frame");
    c.max_width = 20;
    c.trunc_by_end_flag = false;
  }
  if (f == "file") {
    ta.AddColumn("id");
    ta.AddColumn("file");
  }
  if (f == "filelist") {
    ta.SetHeaderFlag(false);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable(syd::PrintTable & ta) const
{
  syd::RecordWithHistory::DumpInTable(ta);
  auto f = ta.GetFormat();

  if (f == "default") {
    ta.Set("id", id);
    ta.Set("p", patient->name);
    ta.Set("inj", injection->radionuclide->name);
    if (dicoms.size() == 0) ta.Set("acqui_date", "no_dicom");
    else ta.Set("acqui_date", dicoms[0]->acquisition_date);
    ta.Set("tags", GetLabels(tags));
    ta.Set("size", syd::ArrayToString<int, 3>(size));
    ta.Set("spacing", syd::ArrayToString<double, 3>(spacing));
    std::string dicom;
    for(auto d:dicoms) dicom += syd::ToString(d->id)+" ";
    if (dicom.size() != 0) dicom.pop_back(); // remove last space
    ta.Set("dicom", dicom);
    if (pixel_value_unit != NULL) ta.Set("unit", pixel_value_unit->name);
    ta.Set("ref_frame", frame_of_reference_uid);
  }

  if (f == "timing") {
    ta.Set("id", id);
    ta.Set("p", patient->name);
    ta.Set("tags", GetLabels(tags));
    double t = GetHoursFromInjection();
    ta.Set("t", t);
  }

  if (f == "history") {
    ta.Set("id", id);
    ta.Set("p", patient->name);
    if (dicoms.size() == 0) ta.Set("acqui_date", "no_dicom");
    else ta.Set("acqui_date", dicoms[0]->acquisition_date);
    ta.Set("tags", GetLabels(tags));
    if (pixel_value_unit != NULL) ta.Set("unit", pixel_value_unit->name);
    ta.Set("ref_frame", frame_of_reference_uid);
  }

  if (f == "file") {
    ta.Set("id", id);
    if (files.size() != 0) ta.Set("file", files[0]->GetAbsolutePath(db_));
  }

  if (f == "filelist") {
    if (files.size() != 0) ta.GetCurrentOutput() << files[0]->GetAbsolutePath(db_) << " ";
  }
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
