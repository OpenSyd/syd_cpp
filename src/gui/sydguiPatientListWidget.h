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

#ifndef SYDGUIPATIENTLISTWIDGET_H
#define SYDGUIPATIENTLISTWIDGET_H

#include "sydguiListWidgetBase.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace sydgui {

  class PatientListWidget:
    public ListWidgetBase<syd::Patient> {
  protected:
    virtual std::string GetLabel(syd::Patient::pointer patient);
  };
}

#endif // SYDGUIPATIENTLISTWIDGET_H
