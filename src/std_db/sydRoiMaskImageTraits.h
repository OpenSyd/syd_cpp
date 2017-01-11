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

#ifndef SYDROIMASKIMAGETRAITS_H
#define SYDROIMASKIMAGETRAITS_H

// syd
#include "sydRoiMaskImage.h"
#include "sydStandardDatabase.h"
#include "sydImageTraits.h"

// --------------------------------------------------------------------
namespace syd {

  /// Main GetTraits function
  DEFINE_TABLE_TRAITS_HEADER(RoiMaskImage);

  /// Specific Sort for Tag
  //DEFINE_TABLE_TRAITS_SORT_HEADER(RoiMaskImage);

}
// --------------------------------------------------------------------

#endif
