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

#ifndef SYDGUIINJECTIONLISTWIDGET_H
#define SYDGUIINJECTIONLISTWIDGET_H

#include "sydguiListWidgetBase.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace sydgui {

  class InjectionListWidget:
    public ListWidgetBase<syd::Injection> {
  public:
    InjectionListWidget();
    bool NewFrame(std::string label,
                  syd::Injection::pointer * injection,
                  syd::Patient::pointer patient);
  protected:
    void Update(syd::Patient::pointer patient);
    virtual std::string GetLabel(syd::Injection::pointer injection);
    syd::Patient::pointer previous_patient;
  };
}

#endif // SYDGUIINJECTIONLISTWIDGET_H
