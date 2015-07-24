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

#ifndef SYDDATABASESCHEMANAME_H
#define SYDDATABASESCHEMANAME_H

#include "sydCommon.h"
#include "sydPrintTable.h"
#include "sydTableElementBase.h"

namespace syd {

  /// Simple native SQL request to get the name stored in the
  /// schema_version table.
  /// See http://www.codesynthesis.com/pipermail/odb-users/2015-March/002464.html
#pragma db view table("schema_version")
  struct schema_version
  {
#pragma db column("name")
    std::string name;
  };

} // end namespace

#endif
