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
#include "sydFileBuilder.h"

// --------------------------------------------------------------------
syd::File::pointer syd::FileBuilder::NewFile(std::string extension)
{
  syd::File::pointer file;
  db_->New(file);
  std::string f = syd::CreateTemporaryFile(db_->GetDatabaseAbsoluteFolder(), extension);
  file->filename = syd::GetFilenameFromPath(f);
  return file;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FileBuilder::RenameFile(syd::File::pointer file,
                                  std::string relative_path,
                                  std::string filename,
                                  bool renameFileOnDiskFlag)
{
  // Check directory or create it
  db_->CheckOrCreateRelativePath(relative_path);

   // Compute paths
  std::string new_absolute_path = db_->ConvertToAbsolutePath(relative_path+PATH_SEPARATOR+filename);
  std::string old_absolute_path = db_->GetAbsolutePath(file);

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
  file->path = relative_path;
  file->filename = filename;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FileBuilder::CopyFile(syd::File::pointer file, std::string absolute_path)
{
  std::string path = db_->GetAbsolutePath(file);
  fs::copy_file(absolute_path, path);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FileBuilder::RenameMHDFile(syd::File::pointer mhd_file,
                                     syd::File::pointer raw_file,
                                     std::string relative_path,
                                     std::string filename)
{
  // Rename mhd file
  std::string old_path = db_->GetAbsolutePath(mhd_file);
  RenameFile(mhd_file, relative_path, filename, false); // do not move on disk
  std::string new_path = db_->GetAbsolutePath(mhd_file);

  // Move file on disk
  syd::RenameMHDImage(old_path, new_path);

  // Rename raw file
  std::string f = filename;
  syd::Replace(f, ".mhd", ".raw");
  RenameFile(raw_file, relative_path, f, false); // do not move on disk
}
// --------------------------------------------------------------------
