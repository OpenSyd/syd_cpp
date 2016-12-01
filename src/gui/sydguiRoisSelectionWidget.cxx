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

#include "sydguiRoisSelectionWidget.h"
#include "sydRoiMaskImageHelper.h"

// --------------------------------------------------------------------
sydgui::RoisSelectionWidget::RoisSelectionWidget(syd::RoiMaskImage::vector & r):
  selected_rois(r)
{
  DD("constructor");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool sydgui::RoisSelectionWidget::NewFrame()
{
  ImGui::Text("Rois");
  int i=0;
  ImGui::Columns(3);
  selected_rois.clear();
  bool changed = false;
  for(auto roi:all_rois) {
    ImGui::PushID(i); // require to separate every checkbox ROI
    changed = ImGui::Checkbox("ROI", &selected[i]) || changed;
    ImGui::SameLine();
    ImGui::Text("%i Id %lu : %s %d", i, roi->id, roi->roitype->name.c_str(), selected[i]);
    if (selected[i]) selected_rois.push_back(roi);
    ++i;
    ImGui::PopID();
    ImGui::NextColumn();
  }
  ImGui::Columns(1);
  return changed;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::RoisSelectionWidget::SetImage(syd::Image::pointer im)
{
  image = im;
  all_rois.clear();
  all_rois = syd::FindAllRoiMaskImages(image);
  selected.resize(all_rois.size());
  for(auto & s:selected) s = false;
}
// --------------------------------------------------------------------
