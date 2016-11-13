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

#include "sydguiImagesSelectionWidget.h"
#include "sydRoiMaskImageHelper.h"

// --------------------------------------------------------------------
sydgui::ImagesSelectionWidget::ImagesSelectionWidget(syd::StandardDatabase * d):db(d)
{
  db->Query(all_images);
  selected_images = all_images;
  selected_i = -1;
  selected_id = -1;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool sydgui::ImagesSelectionWidget::NewFrame()
{
  static char filter_include_text[256];
  bool changed = ImGui::InputText("Include", filter_include_text, 256);

  static char filter_exclude_text[256];
  changed = ImGui::InputText("Exclude", filter_exclude_text, 256) || changed;

  if (changed) {
    std::vector<std::string> patterns;
    std::vector<std::string> exclude; // FIXME TODO
    std::string s(filter_include_text);
    syd::GetWords(patterns, s);
    std::string ss(filter_exclude_text);
    syd::GetWords(exclude, ss);
    selected_images.clear();
    db->Grep(selected_images, all_images, patterns, exclude);
  }

  // Keyboard arrow to change selected row
  ImGuiIO& io = ImGui::GetIO();
  static auto key_down_arrow = io.KeyMap[ImGuiKey_DownArrow];
  static auto key_up_arrow = io.KeyMap[ImGuiKey_UpArrow];
  if (selected_id != -1) {
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) {
      if (ImGui::IsKeyPressed(i)) {
        if (i==key_down_arrow) {
          std::min(++selected_i, (int)selected_images.size()-1);
          selected_id = selected_images[selected_i]->id;
          changed = true;
          continue;
        }
        if (i==key_up_arrow) {
          std::max(0,--selected_i);
          selected_id = selected_images[selected_i]->id;
          changed = true;
          continue;
        }
      }
    }
  }

  int i=1;
  int nb_of_selected = 0;
  for(auto image:selected_images) {
    ImGui::PushID(i); // require to separate every checkbox ROI
    //    ImGui::SetKeyboardFocusHere();
    bool selected(image->id == selected_id);
    changed = PrintInfoImage(i, image, selected) || changed;
    if (selected) ++nb_of_selected;
    ImGui::PopID();
    ++i;
  }

  // if the previously selected is not present (filtered), reset the selection
  if (nb_of_selected == 0) {
    selected_id = selected_i = -1;
    changed = true;
  }

  return changed;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool sydgui::ImagesSelectionWidget::PrintInfoImage(int i,
                                                   syd::Image::pointer image,
                                                   bool & selected)
{
  std::stringstream ss;
  ss << i << ". " << image->ToString();

  bool previous(selected);
  bool changed = ImGui::Selectable(ss.str().c_str(), &selected);

  if (selected) {
    selected_id = image->id;
    selected_i = i-1; // because start at 1
  }


  if (ImGui::IsItemActive()) {// NO
    DD(i);
  }

  return changed;//(previous != selected);

  /*
    columns:
    i id patient_name acqui_date tags injection type size spacing pixel_type dicom

    for(auto c:column) {
    ss << c.value;
    }
  */

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer sydgui::ImagesSelectionWidget::GetSelectedImage() const
{
  if (selected_id != -1) {
    return selected_images[selected_i];
  }
  return nullptr;
}
// --------------------------------------------------------------------
