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

#ifndef SYDFILEBUILDER_H
#define SYDFILEBUILDER_H

// syd
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This base class is used to create FileBuilder class that perform operation on Images
  /// series and images.
  class FileBuilder {

  public:
    /// Constructor.
    FileBuilder(syd::StandardDatabase * db) { SetDatabase(db); }

    /// Set the pointer to the database
    void SetDatabase(syd::StandardDatabase * db) { db_ = db; }

    /// Create a new file (random unique filename). Not inserted in the db.
    syd::File::pointer NewFile(std::string extension="");

    /// Copy the content of absolute_path inside the file. DB not updated.
    void CopyFile(syd::File::pointer file, std::string absolute_path, bool overwriteIfExist=false);

     /// Change the filename. DB not updated.
    void RenameFile(syd::File::pointer file,
                    std::string relative_path,
                    std::string filename,
                    bool renameFileOnDiskFlag = true);

    /// Change the filename of a mhd. DB not updated.
    void RenameMHDFile(syd::File::pointer mhd_file,
                       syd::File::pointer raw_file,
                       std::string relative_path,
                       std::string filename);

  protected:
    /// Protected constructor. No need to use directly.
    FileBuilder();

    /// Pointer to the database
    syd::StandardDatabase * db_;

  }; // class FileBuilder

} // namespace syd
// --------------------------------------------------------------------

#endif
