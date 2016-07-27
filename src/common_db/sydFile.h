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

#ifndef SYDFILE_H
#define SYDFILE_H

// syd
#include "sydRecord.h"
#include "sydFileUtils.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::File") callback(Callback)
  /// Store information about a file linked to a database.
  class File: public syd::Record,
              public std::enable_shared_from_this<File> {
  public:

    virtual ~File();

    /// File name
    std::string filename;

    /// File (relative) path
    std::string path;

    /// Associated md5 (not always computed)
    std::string md5;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(File, syd::File);
    // ------------------------------------------------------------------------

    /// Write the element as a string
    virtual std::string ToString() const;

    void Callback(odb::callback_event, odb::database&) const;
    void Callback(odb::callback_event, odb::database&);

    virtual void DumpInTable(syd::PrintTable & table) const;
    virtual void DumpInTable_default(syd::PrintTable & table) const;
    virtual void DumpInTable_file(syd::PrintTable & table) const;
    virtual void DumpInTable_md5(syd::PrintTable & table) const;

    /// Return the full absolute path of the file inside the db (must be persistent)
    std::string GetAbsolutePath() const;

    virtual syd::CheckResult Check() const;

    /// Rename the associated file. Warning, could leave the db in a
    /// wrong state if the file on disk and the object is not updated
    /// accordingly in the db. In doubt use renameFileOnDiskFlag=true,
    /// updateDBFlag=true
    void RenameFile(std::string relative_path,
                    std::string filename,
                    bool renameFileOnDiskFlag,
                    bool updateDBFlag);
  protected:
    File();

    void SetFilenamesToErase() const;

  }; // end of class
} // end namespace
// --------------------------------------------------------------------

#endif
