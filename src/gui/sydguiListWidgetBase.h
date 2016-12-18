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

#ifndef SYDGUILISTFWIDGETBASE_H
#define SYDGUILISTFWIDGETBASE_H

#include "sydgui.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace sydgui {

  template<class RecordType>
    class ListWidgetBase {
  public:
    ListWidgetBase(bool allow_empty=false);
    void SetAllowEmptyFlag(bool b) { allow_empty = b; }
    bool NewFrame(std::string label, typename RecordType::pointer * p);
    bool NewFrame(std::string label, typename RecordType::pointer * p, syd::Database * db);

  protected:
    void UpdateCurrentItem(typename RecordType::pointer p);
    void Update(syd::Database * db);
    void UpdateList();
    virtual std::string GetLabel(typename RecordType::pointer p) = 0;
    typename RecordType::pointer previous_item;
    typename RecordType::vector list;
    std::vector<std::string> items;
    int current_item_num;
    bool allow_empty;
    syd::Database * previous_db;
  };
} // end namespace

#include "sydguiListWidgetBase.txx"
// --------------------------------------------------------------------
#endif // SYDGUILISTFWIDGETBASE_H
