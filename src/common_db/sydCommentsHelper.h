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

#ifndef SYDCOMMENTSHELPER_H
#define SYDCOMMENTSHELPER_H

// syd
#include "sydDatabase.h"
#include "sydRecordWithTags.h"

// --------------------------------------------------------------------
namespace syd {

  /// This file contains helpers function that are hopefully helpful
  /// for syd::RecordWithComments table

  /// Update tag list
  template<class ArgsInfo>
  static void SetCommentsFromCommandLine(std::vector<std::string> & comments,
                                         const syd::Database * db,
                                         ArgsInfo & args_info);

} // namespace syd

#include "sydCommentsHelper.txx"
// --------------------------------------------------------------------

#endif
