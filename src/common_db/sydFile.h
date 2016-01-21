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

// --------------------------------------------------------------------
namespace syd {

#pragma db model version(1, 1)

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::File") callback(Callback)
  /// Store information about a file linked to a database.
  class File: public syd::Record {
  public:

    /// File name
    std::string filename;

    /// File (relative) path
    std::string path;

    /// Associated md5 (not always computed)
    std::string md5;

     // ------------------------------------------------------------------------
    TABLE_DEFINE(File, "syd::File");
    TABLE_DECLARE_MANDATORY_FUNCTIONS(File);
    TABLE_DECLARE_OPTIONAL_FUNCTIONS(File);
    // ------------------------------------------------------------------------

    void Callback(odb::callback_event, odb::database&) const;
    void Callback(odb::callback_event, odb::database&);
    void EraseAssociatedFile() const;

    virtual void InitTable(syd::PrintTable & table) const;
    virtual void DumpInTable(syd::PrintTable & table) const;

    std::string GetAbsolutePath(const syd::Database * db) const;

    virtual syd::CheckResult Check() const;

    // FIXME
    virtual void InitTableDescription(syd::TableDescription * description) const;

  protected:
    File();

  }; // end of class
} // end namespace
// --------------------------------------------------------------------

#endif