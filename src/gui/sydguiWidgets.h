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

#ifndef SYDGUIWIDGETS_H
#define SYDGUIWIDGETS_H

#include "sydgui.h"

// --------------------------------------------------------------------
namespace ImGui
{
  // https://eliasdaler.github.io/using-imgui-with-sfml-pt2#combobox-listbox
  bool Combo(const char* label, int* currIndex, std::vector<std::string>& values);
  bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values);
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
namespace sydgui
{
  void NonEditableFieldWidget(std::string name, std::string value);
  void NonEditableFieldWidget(std::string name, int i);
  void NonEditableFieldWidget(std::string name, double v, int precision);
  bool TextFieldWidget(std::string name, std::string & value,
                       ImGuiInputTextFlags flags=ImGuiInputTextFlags_AutoSelectAll);
  bool TextFieldWidget(int id, std::string name, std::string & value,
                       ImGuiInputTextFlags flags=ImGuiInputTextFlags_AutoSelectAll);
  bool TagsFieldsWidget(syd::Database * db,
                        syd::Tag::vector & tags,
                        std::string name = "Tags");
  void string2char(char ** c, const std::string & s, const int max_size=256);
}
// --------------------------------------------------------------------

#endif // SYDGUINONEDITABLEFIELDWIDGET_H
