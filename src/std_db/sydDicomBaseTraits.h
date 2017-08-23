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

#ifndef SYDDICOMBASETRAITS_H
#define SYDDICOMBASETRAITS_H

// syd
#include "sydDicomBase.h"
#include "sydRecordTraits.h"

// --------------------------------------------------------------------
namespace syd {

  /* Not clear.
     - Cannot be a rela traits because DicomBase not a real table
     - cannot set file_map because BuildFields is const
     - BuildFields is const because lasy initialisation + mutable
   */

} // end of namespace
// --------------------------------------------------------------------

#endif
