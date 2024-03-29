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

#ifndef SYDFILEHELPER_H
#define SYDFILEHELPER_H

// syd
#include "sydFile.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This file contains helpers function that are hopefully helpful
  /// to create and update syd::File table. All functions are static
  /// in a class for clarity.

  /// Simple file creation (not inserted)
  syd::File::pointer NewFile(syd::Database * db,
                             std::string path,
                             std::string filename);

  std::string GetFilenames(syd::File::vector files);
  std::string GetRelativeFolder(syd::File::vector files);

} // namespace syd
// --------------------------------------------------------------------

#endif
