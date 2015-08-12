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
  sydLOG(FATAL) << "Not possible in insert file directly";
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
void syd::File::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event,db);
  if (event == odb::callback_event::pre_erase) {
    EraseAssociatedFile();
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::File::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event,db);
  if (event == odb::callback_event::pre_erase) {
    EraseAssociatedFile();
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::File::EraseAssociatedFile() const
{
  //  syd::StandardDatabase * db = static_cast<syd::StandardDatabase*>(db_);
  std::string p = db_->ConvertToAbsolutePath(path+PATH_SEPARATOR+filename);
  if (std::remove(p.c_str()) != 0) {
    sydLOG(WARNING) << "Could not delete the file " << p;
  }
}
// --------------------------------------------------
