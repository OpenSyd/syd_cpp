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
#include "sydTableElementBase.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db model version(1, 1)

#pragma db object table("File")
  /// Store information about a file linked to a database.
  class File: public syd::TableElementBase {
  public:

#pragma db id auto
    /// Main key (automated, unique)
    IdType id;

    /// File name
    std::string filename;

    /// File (relative) path
    std::string path;

    /// Associated md5 (not always computed)
    std::string md5;

    // ------------------------------------------------------------------------
    SET_TABLE_NAME("File")
    File();

    virtual std::string ToString() const;

    bool operator==(const File & p);
    bool operator!=(const File & p) { return !(*this == p); }

    virtual void OnDelete(syd::Database * db);

  }; // end of class
}
// --------------------------------------------------------------------

#endif
