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
#include "sydTag.h"

// --------------------------------------------------------------------
syd::Image::Image():syd::Record()
{
  type = "unset";
  pixel_type = "unset";
  dimension = 3;
  frame_of_reference_uid = "unset";
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
     << name << " ";
  if (files.size() == 0) ss << "(no files) ";
  //for(auto f:files) ss << f->filename << " "; // only first is usually useful
  else ss << files[0]->filename << " ";
  ss << GetLabels(tags) << " " << type << " " << pixel_type << " "
     << size[0] << "x" << size[1] << "x" << size[2] << " "
     << spacing[0] << "x" << spacing[1] << "x" << spacing[2];
  if (dicoms.size() > 0) ss << " " << dicoms[0]->dicom_modality << " ";
  for(auto d:dicoms) ss << d->id << " ";
  ss << frame_of_reference_uid << " ";
  ss << pixel_value_unit->name;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
void syd::Image::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  LOG(FATAL) << "To insert Image, please use sydInsertImageFromDicom";
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::CopyFrom(const pointer p)
{
  syd::Record::CopyFrom(p);
  patient = p->patient;
  files.clear();
  tags.clear();
  dicoms.clear();
  for(auto f:p->files) files.push_back(f);
  for(auto t:p->tags) tags.push_back(t);
  for(auto d:p->dicoms) dicoms.push_back(d);
  type = p->type;
  pixel_type = p->pixel_type;
  dimension = p->dimension;
  frame_of_reference_uid = p->frame_of_reference_uid;
  for(auto i=0; i<size.size(); i++) size[i] = p->size[i];
  for(auto i=0; i<spacing.size(); i++) spacing[i] = p->spacing[i];
  pixel_value_unit = p->pixel_value_unit;
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Image::IsEqual(const pointer p) const
{
  bool b = (syd::Record::IsEqual(p) and
            patient->id == p->patient->id);
  for(auto i=0; i<files.size(); i++) b = b and files[i]->id == p->files[i]->id;
  for(auto i=0; i<tags.size(); i++) b = b and tags[i]->id == p->tags[i]->id;
  for(auto i=0; i<dicoms.size(); i++) b = b and dicoms[i]->id == p->dicoms[i]->id;
  b  =  b and
    type == p->type and
    pixel_type == p->pixel_type and
    dimension == p->dimension and
    pixel_value_unit == p->pixel_value_unit and
    frame_of_reference_uid == p->frame_of_reference_uid;
  for(auto i=0; i<size.size(); i++) b = b and size[i] == p->size[i];
  for(auto i=0; i<spacing.size(); i++) b = b and spacing[i] == p->spacing[i];
  return b;
}
// --------------------------------------------------


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
void syd::Image::AddTag(syd::Tag::pointer tag)
{
  bool found = false;
  int i=0;
  while (i<tags.size() and !found) {
    if (tags[i]->id == tag->id) found = true;
    ++i;
  }
  if (!found) tags.push_back(tag);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::RemoveTag(syd::Tag::pointer tag)
{
  bool found = false;
  int i=0;
  while (i<tags.size() and !found) {
    if (tags[i]->id == tag->id) {
      found = true;
      tags.erase(tags.begin()+i);
    }
    ++i;
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'Image': " << std::endl
              << "\tdefault: id patient tags size spacing dicoms" << std::endl
              << "\tfile: full path" << std::endl
              << "\tfilelist: full path as a line" << std::endl;
    return;
  }
  if (format == "file") {
    ta.AddColumn("#file", 100);
  }
  else {
    if (format == "filelist") {
      // no column
    }
    else {
      ta.AddColumn("#id", 5);
      ta.AddColumn("p", 8);
      ta.AddColumn("acqui_date", 18);
      ta.AddColumn("tags", 40);
      ta.AddColumn("size", 12);
      ta.AddColumn("spacing", 25);
      ta.AddColumn("dicom_id", 15);
      ta.AddColumn("unit", 12);
      ta.AddColumn("ref_frame", 20, 0, false);
    }
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable(const syd::Database * d,
                             syd::PrintTable & ta,
                             const std::string & format) const
{
  if (format == "file") {
    if (files.size() == 0) {
      ta << "no_file";
      return;
    }
    ta << files[0]->GetAbsolutePath(d);
  }
  else {
    if (format == "filelist") {
      std::cout << files[0]->GetAbsolutePath(d) << " ";
    }
    else {
      ta << id << patient->name;
      if (dicoms.size() == 0) ta << "no_dicom";
      else ta << dicoms[0]->acquisition_date;
      ta << GetLabels(tags)
         << syd::ArrayToString<int, 3>(size) << syd::ArrayToString<double, 3>(spacing);
      std::string dicom;
      for(auto d:dicoms) dicom += syd::ToString(d->id)+" ";
      dicom.pop_back(); // remove last space
      ta << dicom;
      if (pixel_value_unit != NULL) ta << pixel_value_unit->name;
      else ta << "novalue";
      ta << frame_of_reference_uid;
    }
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
void syd::Image::UpdateFile(syd::Database * db,
                            const std::string & path,
                            bool deleteExistingFiles)
{
  std::string mhd_relative_path = ComputeRelativeFolder()+PATH_SEPARATOR;
  std::string mhd_filename = syd::GetFilenameFromPath(path);
  return UpdateFile(db, mhd_filename, mhd_relative_path, deleteExistingFiles);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::UpdateFile(syd::Database * db,
                            const std::string & filename,
                            const std::string & relativepath,
                            bool deleteExistingFiles)
{
  std::string extension = GetExtension(filename);
  type = extension;
  if (files.size() != 0 and deleteExistingFiles) {
    LOG(FATAL) << "TODO UpdateFile deleteExistingFiles";
    //    for(auto f:files)
  }
  files.clear();
  syd::File::pointer f;
  db->New(f);
  f->filename = filename;
  f->path = relativepath;

  // Create folder if needed
  std::string absolute_folder = db->ConvertToAbsolutePath(relativepath);
  fs::path dir(absolute_folder);
  if (!fs::exists(dir)) {
    LOG(4) << "Creating folder: " << absolute_folder;
    if (!fs::create_directories(dir)) {
      LOG(FATAL) << "Error, could not create the folder: " << absolute_folder;
    }
  }

  db->Insert(f);
  files.push_back(f);
  if (extension == "mhd") {
    std::string ff = filename;
    syd::Replace(ff, ".mhd", ".raw");
    db->New(f);
    f->filename = ff;
    f->path = relativepath;
    db->Insert(f);
    files.push_back(f);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event,db);
  if (event == odb::callback_event::post_erase) {
    for(auto f:files) db.erase(f);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event,db);
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
std::string syd::Image::ComputeDefaultAbsolutePath(syd::Database * db) const
{
  // fast initial check (useful but not sufficient)
  if (id == -1) {
    LOG(FATAL) << "Could not compute a default filename for this image, the object is not persistant: " << this;
  }
  std::ostringstream oss;
  if (dicoms.size() != 0) oss << dicoms[0]->dicom_modality;
  else  oss << "image";
  oss << "_" << id << ".mhd";
  std::string mhd_filename = oss.str();
  std::string mhd_relative_path = ComputeRelativeFolder()+PATH_SEPARATOR;
  std::string mhd_path = db->ConvertToAbsolutePath(mhd_relative_path+mhd_filename);
  return mhd_path;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::FatalIfNoDicom() const
{
  if (dicoms.size() == 0) {
    LOG(FATAL) << "Error the following image does not have associated DicomSerie."
               << std::endl << this;
  }
}
// --------------------------------------------------
