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

#ifndef SYDDATABASEPATH_H
#define SYDDATABASEPATH_H

// syd
#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  // template<class TableElement>
  // std::string GetAbsoluteFolder(syd::Database * db, const TableElement & e);

  // template<class TableElement>
  // std::string GetRelativeFolder(syd::Database * db, const TableElement & e);

  template<class TableElement>
  std::string GetAbsoluteFilePath(const syd::Database * db, const TableElement & e);

  template<class TableElement>
  std::string GetRelativeFilePath(const syd::Database * db, const TableElement & e);

  template<class TableElement>
  std::string ComputeRelativeFolder(const syd::Database * db, const TableElement & e);

#include "sydDatabasePath.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
