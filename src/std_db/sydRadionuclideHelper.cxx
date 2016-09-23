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
#include "sydRadionuclideHelper.h"

// --------------------------------------------------------------------
syd::Radionuclide::pointer syd::FindRadionuclide(syd::StandardDatabase * db,
                                                 const std::string & name)
{
  syd::Radionuclide::pointer rad;
  odb::query<syd::Radionuclide> q = odb::query<syd::Radionuclide>::name == name;
  try {
    db->QueryOne(rad, q);
  } catch(std::exception & e) {
    EXCEPTION("Cannot find Radionuclide " << name << std::endl
              << "Error message is: " << e.what());
  }
  return rad;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
