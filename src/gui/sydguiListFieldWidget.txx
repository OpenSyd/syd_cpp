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

#include "sydguiListFieldWidget.h"

// --------------------------------------------------------------------
template<class RecordType>
sydgui::ListFieldWidget<RecordType>::ListFieldWidget()
{
  DD("const ListFieldWidget");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
bool sydgui::ListFieldWidget<RecordType>::
NewFrame()
{
  bool modified = ImGui::Combo(label,
                               &item,
                               const_cast<const char**>(&items[0]),
                               items.size());
  if (modified) *pointer = list[item];
  return modified;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void sydgui::ListFieldWidget<RecordType>::
Init(std::string lab,
     typename RecordType::pointer * p,
     typename RecordType::vector & l)
{
  pointer = p;
  list = l;
  items.resize(list.size());
  int i=0;
  item = -1;
  for(auto r:list) {
    items[i] = (char*)realloc(items[i],256);
    strcpy(items[i], GetLabel(r).c_str());
    if (*pointer != nullptr and r->id == (*pointer)->id) item = i;
    ++i;
  }
  label = (char*)realloc(label, 256);
  strcpy(label, lab.c_str());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
std::string sydgui::ListFieldWidget<RecordType>::
GetLabel(typename RecordType::pointer)
{
  return (*pointer)->ToString();
}
// --------------------------------------------------------------------
