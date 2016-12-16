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
bool sydgui::ImageWidget::NewFrame()
{
  if (image ==nullptr) return false;
  auto db = image->GetDatabase<syd::StandardDatabase>();

  ImGui::Button("View with vv");
  ImGui::SameLine();
  ImGui::Button("Copy path in clipboard");
  ImGui::SameLine();
  ImGui::Button("Delete");
  //ImGui::Button("Copy"); // copy ?
  if (modified) {
    //    ImGui::SameLine();
    ImGui::Button("Update");
    if (ImGui::IsItemActive() || ImGui::IsItemHovered())
      ImGui::SetTooltip("Record has been modified. Click here to save.");
    ImGui::SameLine();
    ImGui::Button("Revert");
  }

  static auto ro_flag = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll;
  static auto rw_flag = ImGuiInputTextFlags_AutoSelectAll;
  static char c[256];

  // Id
  sydgui::NonEditableFieldWidget("Id", image->id);

  // Patient
  bool patient_modified = patient_list_widget.NewFrame();
  modified = patient_modified or modified;

  // injection
  if (patient_modified) {
    UpdateListOfInjections();
    // Remove injection if the patient is different
    if ((image->injection != nullptr) and
        (image->injection->patient->id != image->patient->id))
      image->injection = nullptr;
  }
  modified = injection_list_widget.NewFrame() or modified;

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
  modified = pixel_unit_list_widget.NewFrame() or modified;

  // frame of ref
  modified = sydgui::TextFieldWidget("Frame of ref", image->frame_of_reference_uid) or modified;

  // Image properties
  sydgui::NonEditableFieldWidget("Pixel Type", image->pixel_type);
  sydgui::NonEditableFieldWidget("Dim", image->dimension);
  sydgui::NonEditableFieldWidget("Size", image->SizeAsString());
  sydgui::NonEditableFieldWidget("Spacing", image->SpacingAsString());

  // Comments //to put as sygui::CommentFieldWidget
  int i=0;
  ImGui::Text("Comments:");
  for(auto & com:image->comments) {
    std::string l = "Com "+std::to_string(i);
    modified = sydgui::TextFieldWidget(l, com) or modified;
    ImGui::SameLine();
    ImGui::Button("Remove");
    ++i;
  }
  if (i==0) ImGui::Text("no comment");
  ImGui::Button("Add a comment");

  // History
  sydgui::NonEditableFieldWidget("Created", image->history->insertion_date);
  sydgui::NonEditableFieldWidget("Last modif", image->history->update_date);

  return modified;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::ImageWidget::SetImage(syd::Image::pointer im)
{
  image = im;
  if (image == nullptr) return;

  // Important: every field with be with id specific to image id
  ImGui::PopID();
  ImGui::PushID(std::to_string(image->id).c_str());

  // Surprisingly fast enough
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::Image::pointer im_db;
  db->QueryOne(im_db, image->id);
  if (im_db->ToString() != image->ToString()) modified = true;
  else modified = false;

  //tag_in_progress_flag = false;
  UpdateListOfPatients();
  UpdateListOfInjections();
  UpdateListOfPixelUnits();

  //tags = syd::GetLabels(image->tags);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::ImageWidget::UpdateListOfPatients()
{
  if (image == nullptr) return;
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::Patient::vector patients;
  db->Query(patients);
  patient_list_widget.Init("Patient", &image->patient, patients);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::ImageWidget::UpdateListOfInjections()
{
  if (image == nullptr) return;
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::Injection::vector injections;
  typedef odb::query<syd::Injection> Q;
  Q q = Q::patient == image->patient->id;
  db->Query(injections, q);
  injection_list_widget.Init("Injection", &image->injection, injections);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::ImageWidget::UpdateListOfPixelUnits()
{
  if (image == nullptr) return;
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::PixelUnit::vector pixel_units;
  db->Query(pixel_units);
  pixel_unit_list_widget.Init("Pixel unit", &image->pixel_unit, pixel_units);
}
// --------------------------------------------------------------------


