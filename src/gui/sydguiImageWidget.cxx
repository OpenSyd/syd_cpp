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
#include "sydguiWidgets.h"

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
  static auto rw_flag = ImGuiInputTextFlags_AutoSelectAll;
  static char c[256];
  bool modified = false;

  // Id
  sydgui::NonEditableFieldWidget("Id", image->id);

  strcpy(c, image->GetPatientName().c_str());
  ImGui::InputText("Patient", c, 256, ro_flag);

  // injection
  sydgui::NonEditableFieldWidget("Injection", image->GetInjectionName().c_str());

  // acq date
  sydgui::NonEditableFieldWidget("Acquisition_date", image->acquisition_date);

  // Modality
  modified = sydgui::TextFieldWidget("Modality", image->modality);

  // Tag
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

  // image type (ignored)
  sydgui::NonEditableFieldWidget("Type", image->type);

  strcpy(c, image->pixel_type.c_str());
  ImGui::InputText("*Pixel type", c, 256, rw_flag);

  if (image->pixel_unit != nullptr) {
    strcpy(c, image->pixel_unit->name.c_str());
    ImGui::InputText("Pixel unit", c, 256, ro_flag);
  }

  strcpy(c, image->frame_of_reference_uid.c_str());
  ImGui::InputText("*Frame of ref.", c, 256, ro_flag);

  // Image properties
  sydgui::NonEditableFieldWidget("Dim", image->dimension);
  sydgui::NonEditableFieldWidget("Size", image->SizeAsString());
  sydgui::NonEditableFieldWidget("Spacing", image->SpacingAsString());

  // Comments //to put as sygui::CommentFieldWidget
  int i=0;
  for(auto com:image->comments) {
    std::string l = "Com "+std::to_string(i);
    sydgui::NonEditableFieldWidget(l, com);
    ++i;
  }
  if (i==0) ImGui::Text("no comment");

  // History
  sydgui::NonEditableFieldWidget("Created", image->history->insertion_date);
  sydgui::NonEditableFieldWidget("Last modif", image->history->update_date);

  // history
  // tag

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

