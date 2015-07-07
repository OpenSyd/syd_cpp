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
syd::Image::Image():Record("")
{
  type = "unset";
  pixel_type = "unset";
  dimension = 3;
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
  ss << GetTagLabels(tags) << " " << type << " " << pixel_type << " "
     << size[0] << "x" << size[1] << "x" << size[2] << " "
     << spacing[0] << "x" << spacing[1] << "x" << spacing[2];
  if (dicoms.size() > 0) ss << " " << dicoms[0]->dicom_modality << " ";
  for(auto d:dicoms) ss << d->id << " ";
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
void syd::Image::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  LOG(FATAL) << "To insert Image, please use sydInsertImage";
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
  for(auto i=0; i<size.size(); i++) size[i] = p->size[i];
  for(auto i=0; i<spacing.size(); i++) spacing[i] = p->spacing[i];
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
    dimension == p->dimension;
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
    if (tags[i]->label == tag->label) found = true;
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
    if (tags[i]->label == tag->label) {
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
              << "\tdefault: id patient tags size spacing dicoms" << std::endl;
    return;
  }
  ta.AddColumn("#id");
  ta.AddColumn("p", 8);
  ta.AddColumn("tags", 20);
  ta.AddColumn("size", 10);
  ta.AddColumn("spacing", 10);
  ta.AddColumn("dicoms", 20);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  ta << id << patient->name << GetTagLabels(tags)
     << syd::ArrayToString<int, 3>(size) << syd::ArrayToString<double, 3>(spacing);
  std::string dicom;
  for(auto d:dicoms) dicom += syd::ToString(d->id)+" ";
  ta << dicom;
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::Image::ComputeRelativeFolder() const
{
  return patient->name;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::Sort(syd::Image::vector & v, const std::string & type)
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              if (a->dicoms.size() == 0) return true;
              if (b->dicoms.size() == 0) return false;
              return a->dicoms[0]->acquisition_date < b->dicoms[0]->acquisition_date;
            });
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
