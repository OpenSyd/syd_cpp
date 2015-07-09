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
#include "sydFile.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------
syd::File::File():syd::Record()
{
  // default value
  filename = "unset"; // must be unique
  path = "unset";
  md5 = "unset";
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::File::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << filename << " "
     << path << " "
     << (md5 == "unset"? "no_md5":"md5");
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::File::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and
          filename == p->filename and
          path == p->path and
          md5 == p->md5);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::File::CopyFrom(const pointer p)
{
  syd::Record::CopyFrom(p);
  filename = p->filename;
  path = p->path;
  md5 = p->md5;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::File::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  LOG(FATAL) << "Not possible in insert file directly";
}
// --------------------------------------------------


// --------------------------------------------------
void syd::File::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'File': " << std::endl
              << "\tdefault: id filename path md5(y/n)" << std::endl
              << "\tmd5: id filename path md5(complete)" << std::endl
              << "\tmpath: id pathname" << std::endl;
    return;
  }
  ta.AddColumn("#id");
  if (format == "md5") {
    ta.AddColumn("filename", 40);
    ta.AddColumn("path", 30);
    ta.AddColumn("md5", 40);
  }
  else {
    if (format == "path") {
      ta.AddColumn("path", 100);
    }
    else {
      ta.AddColumn("filename", 40);
      ta.AddColumn("path", 30);
      ta.AddColumn("md5?", 10);
    }
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::File::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  ta << id;
  if (format == "md5") {
    ta << filename  << path << md5;
  }
  else {
    if (format == "path") {
      ta << std::string(path+PATH_SEPARATOR+filename);
    }
    else {
      ta << filename  << path << (md5=="unset" ? "no_md5":"md5");
    }
  }
}
// --------------------------------------------------


// --------------------------------------------------
// void syd::File::OnDelete(syd::Database * d)
// {
//   DD("File OnDelete");
  /*
  syd::StandardDatabase * db = dynamic_cast<syd::StandardDatabase*>(d);

  // When a File is deleted, we also delete the file on disk
  std::string f = db->GetAbsolutePath(*this);
  db->AddFileToDelete(f);// list_of_files_to_delete.push_back(f);

  // We also look for images to be deleted. Manual search because dont know how to do
  std::vector<syd::Image> images_temp;
  std::vector<syd::Image> images;
  db->Query<syd::Image>(images_temp);
  for(auto i:images_temp) {
    for(auto f:i.files) {
      if (f->id == id) images.push_back(i);
    }
  }
  for(auto i:images) db->AddToDeleteList(i);
  */

  // We also look for DicomFiles to be deleted.
  /* if (look_for_dicomfile_on_delete_flag) {
    std::vector<syd::DicomFile> dicomfiles;
    db->Query<syd::DicomFile>(odb::query<syd::DicomFile>::file == id, dicomfiles);
    for(auto i:dicomfiles) db->AddToDeleteList(i);
  }
  */
// }
// --------------------------------------------------


// --------------------------------------------------
void syd::File::Sort(syd::File::vector & v, const std::string & type) {}
// --------------------------------------------------


// --------------------------------------------------
void syd::File::Callback(odb::callback_event event, odb::database & db) const
{
  if (event == odb::callback_event::pre_erase) {
    EraseAssociatedFile();
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::File::Callback(odb::callback_event event, odb::database & db)
{
  if (event == odb::callback_event::pre_erase) {
    EraseAssociatedFile();
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::File::EraseAssociatedFile() const
{
  std::cout << "TODO File erase " << filename << std::endl;
}
// --------------------------------------------------
