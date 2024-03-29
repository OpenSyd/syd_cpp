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
#include "sydTagHelper.h"

DEFINE_TABLE_IMPL(File);

// --------------------------------------------------------------------
syd::File::File():syd::Record(), syd::RecordWithTags()
{
  // default value
  filename = empty_value;
  path = empty_value;
  md5 = empty_value;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::File::~File()
{
  if (!IsPersistent()) {
    fs::path f(GetAbsolutePath());
    if (fs::exists(f)) {
      LOG(WARNING) << "The file '" << GetAbsolutePath()
                   << "' is not persistent, I delete the (temporary) file.";
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
     << md5//  << " "
    // << syd::GetLabels(tags) << " "
    ;
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
  if (path == empty_value) return "";
  std::string apath = db_->ConvertToAbsolutePath(path+PATH_SEPARATOR+filename);
  return apath;
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


// --------------------------------------------------------------------
void syd::File::RenameFile(std::string relative_path,
                           std::string vfilename,
                           bool renameFileOnDiskFlag,
                           bool updateDBFlag)
{
  // Check directory or create it
  db_->CheckOrCreateRelativePath(relative_path);

  // Compute paths
  std::string new_absolute_path = db_->ConvertToAbsolutePath(relative_path+PATH_SEPARATOR+vfilename);
  std::string old_absolute_path = GetAbsolutePath();

  // If file already exist, mv it. Do nothing if does no exist.
  fs::path old(old_absolute_path);
  if (fs::exists(old) and renameFileOnDiskFlag) {
    // Destination exist ?
    fs::path dir(new_absolute_path);
    if (fs::exists(dir)) {
      EXCEPTION("Could not update file '"
                << old_absolute_path
                << "', destination already exists: '"
                << new_absolute_path << "'.");
    }
    fs::rename(old_absolute_path, new_absolute_path);
  }

  // Update in the db
  path = relative_path;
  filename = vfilename;

  if (updateDBFlag) db_->Update(shared_from_this());
}
// --------------------------------------------------------------------
