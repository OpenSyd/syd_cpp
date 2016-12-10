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

#include "sydguiListFieldWidget.h"

// --------------------------------------------------------------------
template<>
std::string sydgui::ListFieldWidget<syd::Injection>::
GetLabel(syd::Injection::pointer injection)
{
  if (injection->radionuclide == nullptr) return "no_rad";
  std::ostringstream oss;
  oss << "(" << injection->id << ") "
      << injection->radionuclide->name
      << " " << injection->date << " "
      << injection->activity_in_MBq << "MBq";
  return oss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
std::string sydgui::ListFieldWidget<syd::Patient>::
GetLabel(syd::Patient::pointer patient)
{
  return patient->name;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
std::string sydgui::ListFieldWidget<syd::PixelUnit>::
GetLabel(syd::PixelUnit::pointer p)
{
  std::ostringstream oss;
  oss << p->name << " (" << p->description << ")";
  return oss.str();
}
// --------------------------------------------------------------------
