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
syd::PixelUnit::pointer syd::FindPixelUnit(syd::StandardDatabase * db,
                                           std::string unit_name)
{
  syd::PixelUnit::pointer unit;
  try {
    odb::query<syd::PixelUnit> q =
      odb::query<syd::PixelUnit>::name == unit_name;
    db->QueryOne(unit, q);
  } catch (std::exception & e) {
    EXCEPTION("Cannot find the PixelUnit named '" << unit_name << "'");
  }
  return unit;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::PixelUnit::pointer syd::FindOrCreatePixelUnit(syd::StandardDatabase * db,
                                                   std::string unit_name,
                                                   std::string unit_desc)
{
  syd::PixelUnit::pointer unit;
  try {
    unit = syd::FindPixelUnit(db, unit_name);
  } catch (std::exception & e) {
    unit = db->New<syd::PixelUnit>();
    unit->name = unit_name;
    unit->description = unit_desc;
    db->Insert(unit);
  }
  return unit;
}
// --------------------------------------------------------------------
