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

#ifndef SYDGUILISTFIELDWIDGET_H
#define SYDGUILISTFIELDWIDGET_H

#include "sydgui.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace sydgui {

  template<class RecordType>
  class ListFieldWidget {
  public:
    ListFieldWidget();

    bool NewFrame();
    void Init(std::string label,
              typename RecordType::pointer * pointer,
              typename RecordType::vector & list);
    virtual std::string GetLabel(typename RecordType::pointer p);

  protected:
    typename RecordType::pointer * pointer;
    typename RecordType::vector list;
    std::vector<char *> items;
    char * label;
    int item;
  };
}

#include "sydguiListFieldWidget.txx"

// --------------------------------------------------------------------
// Specializations
template<> std::string sydgui::ListFieldWidget<syd::Injection>::
GetLabel(syd::Injection::pointer injection);
template<> std::string sydgui::ListFieldWidget<syd::Patient>::
GetLabel(syd::Patient::pointer injection);
template<> std::string sydgui::ListFieldWidget<syd::PixelUnit>::
GetLabel(syd::PixelUnit::pointer pixel_unit);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
#endif // SYDGUILISTFIELDWIDGET_H
