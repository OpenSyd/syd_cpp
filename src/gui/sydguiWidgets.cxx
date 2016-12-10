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

#include "sydguiWidgets.h"

// --------------------------------------------------------------------
void sydgui::NonEditableFieldWidget(std::string name, std::string value)
{
  // Read Only flag
  static auto ro_flag = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll;
  // Get the value
  static char c[256];
  strcpy(c, value.c_str());
  // To allow mouse selection, we need a different widget ID. Build from the name
  char l[256];
  sprintf(l, "##%s", name.c_str());
  // Widgets:
  ImGui::InputText(l, c, 256, ro_flag);
  ImGui::SameLine();
  ImGui::TextDisabled("%s", name.c_str());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::NonEditableFieldWidget(std::string name, int i)
{
  sydgui::NonEditableFieldWidget(name, std::to_string(i));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::NonEditableFieldWidget(std::string name, double v, int precision)
{
  DD("todo");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool sydgui::TextFieldWidget(int id, std::string name, std::string & value)
{
  return TextFieldWidget(name+"##"+std::to_string(id), value);
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
bool sydgui::TextFieldWidget(std::string name, std::string & value)
{
  static auto rw_flag = ImGuiInputTextFlags_AutoSelectAll;
  static char c[256];
  strcpy(c, value.c_str());
  bool b = ImGui::InputText(name.c_str(), c, 256, rw_flag);
  if (b) value = c;
  return b;
}
// --------------------------------------------------------------------
