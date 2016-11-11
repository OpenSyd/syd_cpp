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

#include "sydguiImagesListWidget.h"
#include "sydRoiMaskImageHelper.h"

// --------------------------------------------------------------------
sydgui::ImagesListWidget::ImagesListWidget(syd::Image::vector im)
{
  DD("constructor");
  SetImages(im);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool sydgui::ImagesListWidget::NewFrame()
{
  ImGui::Text("%lu images", images.size());

  std::stringstream ss;
  for(auto image:images)
    ss << image->id << " ";
  static char c[5000];
  strcpy(c, ss.str().c_str());
  ImGui::Text("Ids: ");
  ImGui::SameLine();
  ImGui::InputText("", c, 5000, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll);

  // info about patients
  for(auto patient:patients)
    ImGui::Text("Patient %s (%lu): %i images",
                patient->name.c_str(),
                patient->id,
                nb[patient->id]);

  return false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::ImagesListWidget::SetImages(syd::Image::vector im)
{
  images = im;
  ids.clear();
  patients.clear();
  nb.clear();
  for(auto image:images) {
    if (std::find(ids.begin(), ids.end(), image->patient->id) == ids.end()) {
      ids.insert(image->patient->id);
      patients.push_back(image->patient);
    }
    ++nb[image->patient->id];
  }
}
// --------------------------------------------------------------------

