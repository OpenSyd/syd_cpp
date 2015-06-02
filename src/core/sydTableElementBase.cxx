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
#include "sydTableElementBase.h"

// --------------------------------------------------------------------
std::string syd::TableElementBase::ToString() const
{
  LOG(FATAL) << "The function 'ToString' must be implemented for this table: ";
  return ""; // avoid warning
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TableElementBase::Set(std::vector<std::string> & arg)
{
  LOG(FATAL) << "The function 'Set' must be implemented for this table.";
}
// --------------------------------------------------------------------
