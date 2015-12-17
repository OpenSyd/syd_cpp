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
//#include "sydStandardDatabase.h"
#include "sydDatabase.h"

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
bool syd::File::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and
          filename == p->filename and
          path == p->path and
          md5 == p->md5);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  LOG(FATAL) << "Not possible in insert file directly";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event,db);
  if (event == odb::callback_event::pre_erase) {
    EraseAssociatedFile();
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event,db);
  if (event == odb::callback_event::pre_erase) {
    EraseAssociatedFile();
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::EraseAssociatedFile() const
{
  //  syd::StandardDatabase * db = static_cast<syd::StandardDatabase*>(db_);
  std::string p = db_->ConvertToAbsolutePath(path+PATH_SEPARATOR+filename);
  if (std::remove(p.c_str()) != 0) {
    LOG(WARNING) << "Could not delete the file " << p;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::File::GetAbsolutePath(const syd::Database * db) const
{
  std::string apath = db->ConvertToAbsolutePath(path+PATH_SEPARATOR+filename);
  return apath;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::InitTable(syd::PrintTable & ta) const
{
  ta.AddFormat("md5", "Display the md5 value");
  ta.AddFormat("file", "Display the complete image path");

  auto & f = ta.GetFormat();
  ta.AddColumn("id");

  if (f == "default" or f == "md5") {
    ta.AddColumn("filename");
    ta.AddColumn("folder");
    if (f == "md5") ta.AddColumn("md5");
    else ta.AddColumn("md5?");
  }
  if (f == "file") ta.AddColumn("file");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::File::DumpInTable(syd::PrintTable & ta) const
{
  auto & f = ta.GetFormat();
  ta.Set("id", id);
  if (f == "default" or f == "md5") {
    ta.Set("filename", filename);
    ta.Set("folder", path);
    if (f == "md5") ta.Set("md5", md5);
    else ta.Set("md5?", (md5=="unset" ? "no_md5":"md5"));
  }
  if (f == "file")
    ta.Set("file", std::string(path+PATH_SEPARATOR+filename));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::File::Check() const
{
  // Test if file exist
  std::string s = GetAbsolutePath(db_);
  syd::CheckResult r;
  if (!fs::exists(s)) {
    r.success = false;
    r.description = "the file "+s+" is not found";
  }
  return r;
}
// --------------------------------------------------------------------
