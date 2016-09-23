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

#ifndef SYDRADIONUCLIDEHELPER_H
#define SYDRADIONUCLIDEHELPER_H

// syd
#include "sydRadionuclide.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This file contains helpers function that are hopefully helpful
  /// for syd::Radionuclide table

  // Search for radionuclide by name
  syd::Radionuclide::pointer FindRadionuclide(syd::StandardDatabase * db,
                                              const std::string & name);

}
// --------------------------------------------------------------------

#endif
