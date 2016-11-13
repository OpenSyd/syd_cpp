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

#include "sydguiImageWidget.h"
#include "sydRoiMaskImageHelper.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
sydgui::ImageWidget::ImageWidget()
{
  image = nullptr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool sydgui::ImageWidget::NewFrame()
{
  if (image ==nullptr) return false;

  //  ImGui::Text("Image : %s", image->ToString().c_str());

  /* Display an image. Actions:
     - status: not updated / updated
     - edit some fields, then update
     - open in vv
     - delete, copy
     patient, injection, files, dicoms, type, pixel_type,
     pixel_unit, frame_of_reference_uid, acquisition_date, modality
     dimension, size, spacing
  */

  static auto ro_flag = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll;
  static char c[256];
  strcpy(c, std::to_string(image->id).c_str());
  ImGui::InputText("Id", c, 256, ro_flag);

  strcpy(c, image->GetPatientName().c_str());
  ImGui::InputText("Patient", c, 256, ro_flag);

  strcpy(c, image->GetInjectionName().c_str());
  ImGui::InputText("Injection", c, 256, ro_flag);

  strcpy(c, image->acquisition_date.c_str());
  ImGui::InputText("Acquisition date", c, 256, ro_flag);

  strcpy(c, image->modality.c_str());
  ImGui::InputText("Modality", c, 256, ro_flag);

  strcpy(c, syd::GetLabels(image->tags).c_str());
  ImGui::InputText("Tags", c, 256, ro_flag);

  if (image->files.size() > 0) {
    std::ostringstream ss;
    for(auto f:image->files) ss << f->filename << " ";
    strcpy(c, ss.str().c_str());
    ImGui::InputText("Files", c, 256, ro_flag);
    strcpy(c, image->files[0]->GetAbsolutePath().c_str());
    ImGui::InputText("Folder", c, 256, ro_flag);
  }

  if (image->dicoms.size() > 0) {
    std::ostringstream ss;
    for(auto d:image->dicoms) ss << d->id << " ";
    strcpy(c, ss.str().c_str());
    ImGui::InputText("Dicoms", c, 256, ro_flag);
  }

  strcpy(c, image->type.c_str());
  ImGui::InputText("Type", c, 256, ro_flag);

  strcpy(c, image->pixel_type.c_str());
  ImGui::InputText("Pixel type", c, 256, ro_flag);

  if (image->pixel_unit != nullptr) {
    strcpy(c, image->pixel_unit->name.c_str());
    ImGui::InputText("Pixel unit", c, 256, ro_flag);
  }

  strcpy(c, image->frame_of_reference_uid.c_str());
  ImGui::InputText("Frame", c, 256, ro_flag);

  strcpy(c, std::to_string(image->dimension).c_str());
  ImGui::InputText("Dim", c, 256, ro_flag);

  strcpy(c, image->SizeAsString().c_str());
  ImGui::InputText("Size", c, 256, ro_flag);

  strcpy(c, image->SpacingAsString().c_str());
  ImGui::InputText("Spacing", c, 256, ro_flag);

  strcpy(c, image->history->update_date.c_str());
  ImGui::InputText("Last modif", c, 256, ro_flag);

  strcpy(c, image->history->insertion_date.c_str());
  ImGui::InputText("Created", c, 256, ro_flag);

  // history
  // tag

  /*  std::stringstream ss;
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
  */
  return false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::ImageWidget::SetImage(syd::Image::pointer im)
{
  image = im;
  /*  if (image == nullptr) {
      DD("nothing selected");
      }
      else {
      DD(image);
      }
  */
}
// --------------------------------------------------------------------

