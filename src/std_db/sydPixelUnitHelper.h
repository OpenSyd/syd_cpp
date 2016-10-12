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

#ifndef SYDPIXELUNITHELPER_H
#define SYDPIXELUNITHELPER_H

// syd
#include "sydPixelUnit.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This file contains helpers function that are hopefully helpful
  /// for syd::PixelUnit table

  // Find a pixel unit by name
  syd::PixelUnit::pointer FindPixelUnit(syd::StandardDatabase * db,
                                        std::string unit);

  // Find a pixel unit by name, create if does not exist
  syd::PixelUnit::pointer FindOrCreatePixelUnit(syd::StandardDatabase * db,
                                                std::string unit,
                                                std::string desc="");

}
// --------------------------------------------------------------------

#endif
