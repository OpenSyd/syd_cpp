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
#include "sydFileHelper.h"

// --------------------------------------------------------------------
syd::Image::Image():syd::RecordWithHistory()
{
  patient = NULL;
  injection = NULL;
  pixel_unit = NULL;
  modality = "image"; // default (not empty_value)
  type = pixel_type = acquisition_date = frame_of_reference_uid = empty_value;
  dimension = 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::~Image()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::GetPatientName() const
{
  std::string name = empty_value;
  if (patient != NULL) name = patient->name;
  return name;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::GetInjectionName() const
{
  std::string inj_name = empty_value;
  if (injection != NULL) inj_name = injection->radionuclide->name;
  return inj_name;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << GetPatientName() << " "
     << GetInjectionName() << " ";
  if (files.size() != 0) ss << files[0]->filename << " "; // only first is usually useful
  else ss << empty_value << " ";
  ss << GetLabels(tags) << " "
     << type << " "
     << pixel_type << " "
     << SizeAsString() << " "
     << SpacingAsString() << " "
     << modality << " ";
  for(auto d:dicoms) ss << d->id << " ";
  if (dicoms.size() == 0) ss << empty_value << " ";
  ss << frame_of_reference_uid << " ";
  if (pixel_unit != NULL) ss << pixel_unit->name;
  else ss << empty_value << " ";
  if (history and print_history_flag_) {
    ss << " " << history->insertion_date << " "
       << history->update_date;
  }
  else ss << empty_value;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
std::string syd::Image::GetAcquisitionDate() const
{
  return acquisition_date;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::AddDicomSerie(const syd::DicomSerie::pointer dicom)
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
void syd::Image::RemoveDicomSerie(const syd::DicomSerie::pointer dicom)
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
  if (patient == NULL) {
    LOG(FATAL) << "Cannot get Image::ComputeRelativeFolder while no patient"
               << " is set (the record is no persistent in the db).";
  }
  auto s = patient->name;
  syd::Replace(s, " ", "_"); // replace space with underscore
  return s;
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::Image::ComputeDefaultMhdFilename() const
{
  if (!IsPersistent()) {
    EXCEPTION("Image must be persistent (in the db) to use ComputeDefaultMhdFilename.");
  }
  std::string s = modality+"_"+syd::ToString(id)+".mhd";
  std::ostringstream oss;
  oss << modality;
  oss << "_" << id << ".mhd";
  return oss.str();
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::Callback(odb::callback_event event,
                          odb::database & db) const
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
  if (event == odb::callback_event::post_persist) {
    if (type == "mhd") { // auto rename file if mhd
      auto image = const_cast<syd::Image*>(this); // remove the const
      image->RenameToDefaultMHDFilename(false);
    }
    // update the files
    for(auto f:files) db.update(f);
  }
  if (event == odb::callback_event::pre_update) {
    // update the files
    for(auto f:files) db.update(f);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::Callback(odb::callback_event event,
                          odb::database & db)
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
void syd::Image::CopyDicomSeries(const syd::Image::pointer image)
{
  dicoms.clear();
  for(auto d:image->dicoms) AddDicomSerie(d);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable(syd::PrintTable & ta) const
{
  auto format = ta.GetFormat();
  if (format == "default") DumpInTable_default(ta);
  else if (format == "short") DumpInTable_short(ta);
  else if (format == "ref_frame") DumpInTable_ref_frame(ta);
  else if (format == "history") DumpInTable_history(ta);
  else if (format == "file") DumpInTable_file(ta);
  else if (format == "filelist") DumpInTable_filelist(ta);
  else if (format == "details") DumpInTable_details(ta);
  else {
    ta.AddFormat("default", "id, date, tags, size etc");
    ta.AddFormat("short", "no size");
    ta.AddFormat("ref_frame", "with dicom_reference_frame");
    ta.AddFormat("history", "with date inserted/updated");
    ta.AddFormat("file", "with complete filename");
    ta.AddFormat("filelist", "not a table a list of filenames");
    ta.AddFormat("details", "all details");
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_short(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", GetPatientName());
  ta.Set("acqui_date", GetAcquisitionDate());
  ta.Set("tags", GetLabels(tags), 100);
  if (pixel_unit != NULL) ta.Set("unit", pixel_unit->name);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_default(syd::PrintTable & ta) const
{
  DumpInTable_short(ta);
  ta.Set("inj", GetInjectionName());
  ta.Set("mod", modality);
  ta.Set("size", syd::ArrayToString(size));
  ta.Set("spacing", syd::ArrayToString(spacing,1));
  std::string dicom;
  for(auto d:dicoms) dicom += syd::ToString(d->id)+" ";
  if (dicom.size() != 0) {
    dicom.pop_back(); // remove last space
    ta.Set("dicom", dicom);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_ref_frame(syd::PrintTable & ta) const
{
  DumpInTable_short(ta);
  ta.Set("ref_frame", frame_of_reference_uid);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_history(syd::PrintTable & ta) const
{
  DumpInTable_short(ta);
  syd::RecordWithHistory::DumpInTable(ta);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_file(syd::PrintTable & ta) const
{
  DumpInTable_short(ta);
  ta.Set("file", GetAbsolutePath(), 100);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_filelist(syd::PrintTable & ta) const
{
  ta.SetSingleRowFlag(true);
  ta.SetHeaderFlag(false);
  ta.Set("file", GetAbsolutePath(), 500);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable_details(syd::PrintTable & ta) const
{
  DumpInTable_default(ta);
  ta.Set("t", type);
  ta.Set("pixel", pixel_type);
  ta.Set("dim", dimension);
  ta.Set("ref_frame", frame_of_reference_uid);
  std::string f;
  for(auto a:files) f += syd::ToString(a->id)+" ";
  if (files.size() != 0) {
    f.pop_back(); // remove last space
    ta.Set("files", f);
  }
  syd::RecordWithHistory::DumpInTable(ta);
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
  if (injection == NULL) return 0.0;
  if (acquisition_date == empty_value) return 0.0;
  std::string inj_date = injection->date;
  double time = syd::DateDifferenceInHours(acquisition_date, inj_date);
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
  if (injection == NULL) {
    LOG(FATAL) << "Cannot Image::GetTimesFromInjection because injection of first image is null";
  }
  std::string starting_date = injection->date;
  for(auto image:sorted_images) {
    double t = syd::DateDifferenceInHours(image->acquisition_date, starting_date);
    times->push_back(t);
  }
  return *times;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::GetAbsolutePath() const
{
  if (files.size() == 0) return empty_value;
  return files[0]->GetAbsolutePath();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Image::RenameToDefaultMHDFilename(bool updateDBFlag)
{
  if (patient == NULL) {
    EXCEPTION("No patient in this image, cannot RenameToDefaultMHDFilename().");
  }
  if (type != "mhd") {
    EXCEPTION("Image type is not 'mhd', cannot RenameToDefaultMHDFilename().");
  }

  // Compute the default filename
  std::ostringstream oss;
  oss << modality;
  oss << "_" << id << ".mhd";
  std::string mhd_filename = oss.str();
  std::string raw_filename = mhd_filename;
  syd::Replace(raw_filename, ".mhd", ".raw");
  std::string mhd_relative_path = ComputeRelativeFolder()+PATH_SEPARATOR;
  std::string mhd_path = db_->ConvertToAbsolutePath(mhd_relative_path+mhd_filename);

  // To rename mhd need to change the content of the linked .raw file.
  // Rename mhd file
  auto mhd_file = files[0];
  auto raw_file = files[1];
  std::string old_path = mhd_file->GetAbsolutePath();
  // do not move on disk (yet), but update the db only if updateDBFlag
  mhd_file->RenameFile(mhd_relative_path, mhd_filename, false, updateDBFlag);

  // Move file on disk
  syd::RenameMHDImage(old_path, mhd_path);

  // Rename raw file
  std::string f = mhd_filename;
  syd::Replace(f, ".mhd", ".raw");
  // do not move on disk, update the db
  raw_file->RenameFile(mhd_relative_path, f, false, updateDBFlag);
}
// --------------------------------------------------------------------
