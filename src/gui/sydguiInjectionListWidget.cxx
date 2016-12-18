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

#include "sydguiInjectionListWidget.h"
#include "sydguiWidgets.h"

// --------------------------------------------------------------------
sydgui::InjectionListWidget::InjectionListWidget():
  ListWidgetBase<syd::Injection>()
{
  previous_patient = nullptr;
  allow_empty = true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool sydgui::InjectionListWidget::NewFrame(std::string label,
                                           syd::Injection::pointer * injection,
                                           syd::Patient::pointer patient)
{
  Update(patient);
  return ListWidgetBase<syd::Injection>::NewFrame(label, injection);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::InjectionListWidget::Update(syd::Patient::pointer patient)
{
  // Update only when patient changed
  if ((previous_patient == nullptr and patient != nullptr) or
      (previous_patient != nullptr and patient == nullptr) or
      previous_patient != patient) {
    list.clear();
    items.clear();
    previous_patient = patient;
    current_item_num = -1; // FIXME needed ?
    if (patient == nullptr) return;
    // Retrieve the list of injection for this patient
    auto db = patient->GetDatabase<syd::StandardDatabase>();
    typedef odb::query<syd::Injection> Q;
    Q q = Q::patient == patient->id;
    db->Query(list, q);
    UpdateList();
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string sydgui::InjectionListWidget::GetLabel(syd::Injection::pointer injection)
{
  if (injection->radionuclide == nullptr) return "no_rad";
  std::ostringstream oss;
  oss << "(" << injection->id << ") "
      << injection->radionuclide->name << " "
      << injection->activity_in_MBq << "MBq "
      << " " << injection->date;
  return oss.str();
}
// --------------------------------------------------------------------
