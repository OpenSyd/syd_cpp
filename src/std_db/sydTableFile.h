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

#ifndef SYDTABLEFILE_H
#define SYDTABLEFILE_H

// syd
#include "sydFile.h"
#include "sydDatabase.h"
#include "sydDatabasePath.h"

// --------------------------------------------------------------------
namespace syd {

  /// Return the folder for this file (relative to the db)
  // std::string GetRelativeFolder(const syd::Database *db, const syd::File & file);

  // /// Return the full path folder for this file in the db //FIXME Change to GetAbsoluteFilePath
  // std::string GetAbsolutePath(const syd::Database *db, const syd::File & file);

  template<class File>
  void InsertNewFile(File & file, syd::Database * db, const std::string filename, const std::string & relative_folder);


  // template<>
  // std::string GetRelativeFolder<syd::File>(syd::Database * db, const syd::File & e);

  template<>
  std::string GetRelativeFilePath<syd::File>(const syd::Database * db, const syd::File & e);

#include "sydTableFile.txx"
}
// --------------------------------------------------------------------

#endif
