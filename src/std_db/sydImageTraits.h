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

#ifndef SYDIMAGETRAITS_H
#define SYDIMAGETRAITS_H

// syd
#include "sydPatientTraits.h"
#include "sydImage.h"
#include "sydRecordTraits.h"

// --------------------------------------------------------------------
namespace syd {

  /// Main GetTraits function
  DEFINE_TABLE_TRAITS_HEADER(Image);

  /// Build the fields
  template<> void syd::RecordTraits<syd::Image>::
    BuildFields(const syd::Database * db) const;

} // end of namespace
// --------------------------------------------------------------------

#endif
