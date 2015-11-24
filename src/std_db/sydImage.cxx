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
syd::Image::Image():syd::Record()
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
  if (pixel_value_unit != NULL) ss << pixel_value_unit->name;
  else ss << "pixel_value_unit_unset";
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
      ta.AddColumn("tags", 50);
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
      if (dicom.size() != 0) dicom.pop_back(); // remove last space
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
void syd::Image::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event,db);
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
  /*
    if (event == odb::callback_event::post_persist) {
    DD("image post persist");
    DD(id);
    //rename ? need StandardDatabase ?
    syd::StandardDatabase * sdb = dynamic_cast<syd::StandardDatabase*>(db_);
    // sdb->Insert(files[0]);
    // const syd::Image::pointer image(this);
    // syd::ImageBuilder builder(sdb);
    // builder.RenameToDefaultFilename(image);
    // sdb->Dump();
    }
  */
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
void syd::Image::FatalIfNoDicom() const
{
  if (dicoms.size() == 0) {
    LOG(FATAL) << "Error the following image does not have associated DicomSerie."
               << std::endl << this;
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::CopyTags(syd::Image::pointer image)
{
  for(auto t:image->tags) AddTag(t);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::CopyDicomSeries(syd::Image::pointer image)
{
  for(auto d:image->dicoms) AddDicomSerie(d);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::InitTable(syd::PrintTable & ta)
{
  // Define the formats
  ta.AddFormat("file", "Display the filename");

  // Set the columns
  std::string f = ta.GetFormat();
  if (f == "default") {
    ta.AddColumn("id", 5);
    ta.AddColumn("p", 8);
    ta.AddColumn("acqui_date", 18);
    ta.AddColumn("tags", 50);
    ta.AddColumn("size", 12);
    ta.AddColumn("spacing", 25);
    ta.AddColumn("dicom", 15);
    ta.AddColumn("unit", 12);
    //ta.AddColumn("ref_frame", 20, 0, false);
  }
  if (f == "file") {
    ta.AddColumn("id", 5);
    ta.AddColumn("file", 8);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable(syd::PrintTable & ta)
{
  auto f = ta.GetFormat(); // sure it exists

  if (f == "default") {
    ta.Set("id", id);
    ta.Set("p", patient->name);
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
    else ta.Set("unit", "-");
    //ta.Set("ref_frame", frame_of_reference_uid);
  }

  if (f == "file") {
    ta.Set("id", id);
    if (files.size() == 0) ta.Set("file", "no_file");
    else ta.Set("file", files[0]->GetAbsolutePath(db_));
  }
}
// --------------------------------------------------
