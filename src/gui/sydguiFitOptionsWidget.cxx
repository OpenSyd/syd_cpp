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

#include "sydguiFitOptionsWidget.h"

// --------------------------------------------------------------------
sydgui::FitOptionsWidget::FitOptionsWidget(syd::TimeIntegratedActivityFitOptions & op):
  options(op)
{
  DD("constructor");
  f2_model = f3_model = f4_model = f4a_model = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool sydgui::FitOptionsWidget::NewFrame()
{
  bool changed = ImGui::SliderFloat("", &options.GetR2MinThreshold(), 0.0f, 1.0f, "R2 min = %.1f");
  changed = ImGui::InputInt("Max iteration", &options.GetMaxNumIterations(), 1, 10) || changed;
  changed = ImGui::Checkbox("restricted tac", &options.GetRestrictedFlag()) || changed;

  // The || must be after the ImGui. If before, ImGui command is ignored, and the UI flick.

  changed = ImGui::Checkbox("f2", &f2_model) || changed;
  ImGui::SameLine();
  changed = ImGui::Checkbox("f3", &f3_model) || changed;
  ImGui::SameLine();
  changed = ImGui::Checkbox("f4a", &f4a_model) || changed;
  ImGui::SameLine();
  changed = ImGui::Checkbox("f4", &f4_model) || changed;

  if (changed) {
    options.ClearModels();
    if (f2_model) options.AddModel("f2");
    if (f3_model) options.AddModel("f3");
    if (f4a_model) options.AddModel("f4a");
    if (f4_model) options.AddModel("f4");
  }

  ImGui::Text("Options : %s", options.ToString().c_str());

  return changed;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::FitOptionsWidget::SetImage(syd::Image::pointer im)
{
  image = im;
  options.SetLambdaDecayConstantInHours(image->injection->GetLambdaDecayConstantInHours());
}
// --------------------------------------------------------------------
