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

// syd
#include "sydPixelUnitHelper.h"

// --------------------------------------------------------------------
syd::PixelValueUnit::pointer syd::PixelUnitHelper::FindPixelUnit(syd::StandardDatabase * db,
                                                                 std::string unit_name)
{
  syd::PixelValueUnit::pointer unit;
  try {
    odb::query<syd::PixelValueUnit> q = odb::query<syd::PixelValueUnit>::name == unit_name;
    db->QueryOne(unit, q);
  } catch (std::exception & e) {
    EXCEPTION("Cannot find the PixelUnit named '" << unit_name << "'");
  }
  return unit;
}
// --------------------------------------------------------------------
