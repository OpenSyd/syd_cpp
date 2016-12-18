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
#include "sydFileHelper.h"
#include "sydguiWidgets.h"

// --------------------------------------------------------------------
sydgui::ImageWidget::ImageWidget()
{
  image = nullptr;
  modified = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool sydgui::ImageWidget::NewFrame() //syd::Image::pointer im)
{
  //  if (im == nullptr) return false;
  if (image == nullptr) return false;
  //if (image == nullptr or image != im) SetImage(im);
  auto db = image->GetDatabase<syd::StandardDatabase>();

  // Important: every field will be with id specific to image id
  ImGui::PushID(std::to_string(image->id).c_str());

  // Display the buttons
  SetButtons();

  // Id
  sydgui::NonEditableFieldWidget("Id", image->id);

  // Patient
  modified = patients_widget.NewFrame("Patient", &image->patient, db) or modified;

  // Injection
  modified =
    injections_widget.NewFrame("Injection", &image->injection, image->patient)
    or modified;

  // acquisition date
  bool date_is_valid =
    image->GetAcquisitionDate() == syd::empty_value or
    syd::IsDateValid(image->acquisition_date);
  if (!date_is_valid) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImColor::HSV(0.0f, 1.0f, 1.0f));
  }
  modified = sydgui::TextFieldWidget("Acquisition_date", image->acquisition_date) or modified;
  if (!date_is_valid) {
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::Text("Format: 2016-08-27 12:33");
      ImGui::EndTooltip();
    }
    ImGui::PopStyleColor();
  }

  // Modality
  modified = sydgui::TextFieldWidget("Modality", image->modality) or modified;

  // Tags
  modified = sydgui::TagsFieldsWidget(db, image->tags) or modified;

  // Files and folder
  sydgui::NonEditableFieldWidget("Files", syd::GetFilenames(image->files));
  sydgui::NonEditableFieldWidget("Folder", syd::GetRelativeFolder(image->files));

  // Dicoms FIXME not editable yet. To change
  std::ostringstream ss;
  for(auto d:image->dicoms) ss << d->id << " ";
  sydgui::NonEditableFieldWidget("Dicoms", ss.str());
 
  // image type (ignored)
  sydgui::NonEditableFieldWidget("Type", image->type);

  // Pixel unit
  modified = pixel_unit_widget.NewFrame("Pixel unit", &image->pixel_unit, db) or modified;

  // frame of ref
  modified = sydgui::TextFieldWidget("Frame of ref", image->frame_of_reference_uid) or modified;

  // Image properties
  sydgui::NonEditableFieldWidget("Pixel Type", image->pixel_type);
  sydgui::NonEditableFieldWidget("Dim", image->dimension);
  sydgui::NonEditableFieldWidget("Size", image->SizeAsString());
  sydgui::NonEditableFieldWidget("Spacing", image->SpacingAsString());

  // History
  sydgui::NonEditableFieldWidget("Created", image->history->insertion_date);
  sydgui::NonEditableFieldWidget("Last modif", image->history->update_date);

  // Comments //to put as sygui::CommentFieldWidget
  int i=0;
  ImGui::Text("Comments:");
  for(auto & com:image->comments) {
    std::string l = "##"+std::to_string(i);
    modified = sydgui::TextFieldWidget(l, com) or modified;
    ImGui::SameLine();
    ImGui::Button("Remove");
    ++i;
  }
  std::string new_comment;
  sydgui::TextFieldWidget("", new_comment);
  ImGui::SameLine();
  ImGui::SmallButton("Add a comment");

  ImGui::PopID();
  return modified;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::ImageWidget::SetImage(syd::Image::pointer im)
{
  image = im;
  if (image == nullptr) return;

  // Check if the current image is the same than the one in the db. 
  // Surprisingly fast enough
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::Image::pointer im_db;
  db->QueryOne(im_db, image->id);
  if (im_db->ToString() != image->ToString()) modified = true;
  else modified = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::ImageWidget::SetButtons()
{
  // Button VV
  if (ImGui::Button("View with vv")) {
    std::ostringstream oss;
    oss << "vv " << image->GetAbsolutePath() << "&";
    int r = system(oss.str().c_str());
  }

  // Button copy path
  ImGui::SameLine();
  ImGui::Button("Copy path in clipboard");

  // Button delete
  ImGui::SameLine();
  ImGui::Button("Delete");

  //ImGui::Button("Copy"); // copy ?

  // If image has been modified, display some additional buttons
  if (modified) {
    // Update
    ImGui::SameLine();
    bool udpate = ImGui::Button("Update");
    if (ImGui::IsItemActive() || ImGui::IsItemHovered())
      ImGui::SetTooltip("Record has been modified. Click here to save.");

    // Revert
    ImGui::SameLine();
    bool revert = ImGui::Button("Revert");
    if (ImGui::IsItemActive() || ImGui::IsItemHovered())
      ImGui::SetTooltip("Record has been modified. Click here to revert like in the db");
    if (revert) {
      auto db = image->GetDatabase<syd::StandardDatabase>();
      syd::Image::pointer temp;
      db->QueryOne(temp, image->id);
      // Need to copy the content to keep the same pointer
      *image = *temp;
      modified = false;
    }
  }
}
// --------------------------------------------------------------------
