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
#include "sydTagHelper.h"

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


// --------------------------------------------------------------------
void sydgui::string2char(char ** c, const std::string & s, int max_size)
{
  *c = (char*)realloc(*c,max_size);
  strcpy(*c, s.c_str());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool sydgui::TagsFieldsWidget(syd::Database * db,
                              syd::Tag::vector & tags,
                              std::string name)
{
  auto tag_names = syd::GetLabels(tags);

  static bool tag_in_progress_flag = false;
  if (tag_in_progress_flag) {
    // Use red color to indicate that some text have not been recognized
    ImGui::PushStyleColor(ImGuiCol_Text, ImColor::HSV(0.0f, 1.0f, 1.0f));
  }

  bool tag_modified = sydgui::TextFieldWidget(name.c_str(), tag_names);
  if (!ImGui::IsItemActive() and tag_in_progress_flag) {
    tag_in_progress_flag = false;
    ImGui::PopStyleColor();
  }

  if (tag_in_progress_flag) {
    ImGui::PopStyleColor(); // back to default color
    ImGui::Text("Recognized tags:");
    ImGui::SameLine();
    ImGui::Text("%s", syd::GetLabels(tags).c_str());
  }

  if (tag_modified) {
    std::vector<std::string> tag_words;
    syd::GetWords(tag_words, tag_names);
    syd::Tag::vector new_tags;
    odb::query<syd::Tag> q =
      odb::query<syd::Tag>::label.in_range(tag_words.begin(), tag_words.end());
    db->Query<syd::Tag>(new_tags, q);
    tag_in_progress_flag = (new_tags.size() != tag_words.size());
    tags = new_tags;
    return true;
  }
  return false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// https://eliasdaler.github.io/using-imgui-with-sfml-pt2#combobox-listbox
static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
  auto& vector = *static_cast<std::vector<std::string>*>(vec);
  if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
  *out_text = vector.at(idx).c_str();
  return true;
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool ImGui::Combo(const char* label, int* currIndex, std::vector<std::string>& values)
{
  if (values.empty()) { return false; }
  return Combo(label, currIndex, vector_getter,
               static_cast<void*>(&values), values.size());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool ImGui::ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
{
  if (values.empty()) { return false; }
  return ListBox(label, currIndex, vector_getter,
                 static_cast<void*>(&values), values.size());
}
// --------------------------------------------------------------------
