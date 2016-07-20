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
#include "sydDatabase.h"
#include "sydPrintTable2.h"

// --------------------------------------------------------------------
syd::File::File():syd::Record()
{
  // default value
  filename = "unset";
  path = "";
  md5 = "unset";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::File::~File()
{
  DD("File destructor");
  if (!IsPersistent()) {
    DD("File is not persistent, I try to remove temporary file");
    DD(GetAbsolutePath());
    fs::path f(GetAbsolutePath());
    if (fs::exists(f)) {
      DD("delete file");
      fs::remove(f);
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::File::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << filename << " "
     << path << " "
     << (md5 == "unset"? "no_md5":"md5");
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event,db);
  if (event == odb::callback_event::post_erase) {
    SetFilenamesToErase();
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event,db);
  if (event == odb::callback_event::post_erase) {
    SetFilenamesToErase();
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::SetFilenamesToErase() const
{
  db_->AddFilenameToDelete(db_->ConvertToAbsolutePath(path+PATH_SEPARATOR+filename));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::File::GetAbsolutePath() const
{
  std::string apath = db_->ConvertToAbsolutePath(path+PATH_SEPARATOR+filename);
  return apath;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::DumpInTable(syd::PrintTable2 & ta) const
{
  auto format = ta.GetFormat();

  // format: default, md5, path
  if (format == "default") DumpInTable_default(ta);
  else if (format == "file") DumpInTable_file(ta);
  else if (format == "md5") DumpInTable_md5(ta);
    else {
    ta.AddFormat("default", "id, date, tags, size etc");
    ta.AddFormat("file", "with complete filename");
    ta.AddFormat("md5", "with md5 value");
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::DumpInTable_default(syd::PrintTable2 & ta) const
{
  ta.Set("id", id);
  ta.Set("md5?", (md5=="unset" ? "no_md5":"md5"));
  ta.Set("filename", filename, 100);
  ta.Set("folder", path);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::DumpInTable_md5(syd::PrintTable2 & ta) const
{
  ta.Set("id", id);
  ta.Set("md5", md5);
  ta.Set("filename", filename, 100);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::DumpInTable_file(syd::PrintTable2 & ta) const
{
  ta.Set("id", id);
  ta.Set("path", std::string(path+PATH_SEPARATOR+filename), 100);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::File::Check() const
{
  // Test if file exist
  std::string s = GetAbsolutePath();
  syd::CheckResult r;
  if (!fs::exists(s)) {
    r.success = false;
    r.description = "the file "+s+" is not found";
  }
  return r;
}
// --------------------------------------------------------------------
