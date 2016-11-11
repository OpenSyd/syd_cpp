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

  int i=1;
  for(auto image:selected_images) {
    ImGui::PushID(i); // require to separate every checkbox ROI
    PrintInfoImage(i, image);
    ImGui::PopID();
    ++i;
  }

  return changed;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::ImagesSelectionWidget::PrintInfoImage(int i, syd::Image::pointer image)
{
  std::stringstream ss;
  ss << i << ". " << image->ToString();

  bool selected;
  ImGui::Selectable(ss.str().c_str(), selected);

  if (selected) selected_image = image;
  /*
    columns:
    i id patient_name acqui_date tags injection type size spacing pixel_type dicom

    for(auto c:column) {
    ss << c.value;
    }
  */

}
// --------------------------------------------------------------------
