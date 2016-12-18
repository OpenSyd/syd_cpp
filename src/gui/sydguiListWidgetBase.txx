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

#include "sydguiWidgets.h"

// --------------------------------------------------------------------
template<class RecordType>
sydgui::ListWidgetBase<RecordType>::ListWidgetBase(bool allow_empty_)
{
  allow_empty = allow_empty_;
  current_item_num = 0;
  previous_item = nullptr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
bool sydgui::ListWidgetBase<RecordType>::NewFrame(std::string label,
                                                  typename RecordType::pointer * p)
{
  UpdateCurrentItem(*p);
  bool modified = ImGui::Combo(label.c_str(), &current_item_num, items);
  if (modified) {
    if (!allow_empty) *p = list[current_item_num];
    else {
      if (current_item_num == 0) *p = nullptr;
      else *p = list[current_item_num-1];
    }
  }
  return modified;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void sydgui::ListWidgetBase<RecordType>::
UpdateCurrentItem(typename RecordType::pointer item)
{
  if (item == nullptr and previous_item != nullptr) {
    previous_item = nullptr;
    current_item_num = 0;
    return;
  }
  if (item == nullptr and previous_item == nullptr) {
    return;
  }
  if (previous_item == nullptr or
      previous_item != item) {
    previous_item = item;
    int i=0;
    for(auto & inj:list) {
      if (inj->id == item->id) {
        if (allow_empty) current_item_num = i+1;
        else current_item_num = i;
      }
      ++i;
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void sydgui::ListWidgetBase<RecordType>::UpdateList()
{
  // Compute the labels
  if (allow_empty)
    items.push_back(syd::empty_value); // first for no injection
  for(auto & i:list)
    items.push_back(GetLabel(i));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void sydgui::ListWidgetBase<RecordType>::Update(syd::Database * db)
{
  // Update only when patient changed
  if ((previous_db == nullptr and db != nullptr) or
      (previous_db != nullptr and db == nullptr) or
      previous_db != db) {
    list.clear();
    items.clear();
    previous_db = db;
    if (db == nullptr) return;
    // by default, retrieve all items
    db->Query(list);
    UpdateList();
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
bool sydgui::ListWidgetBase<RecordType>::NewFrame(std::string label,
                                                  typename RecordType::pointer * item,
                                                  syd::Database * db)
{
  Update(db);
  return ListWidgetBase<RecordType>::NewFrame(label, item);
}
// --------------------------------------------------------------------


