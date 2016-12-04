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

#ifndef SYDGUIIMAGEWIDGET_H
#define SYDGUIIMAGEWIDGET_H

#include "sydgui.h"
#include "sydguiListFieldWidget.h"
#include "sydStandardDatabase.h"
#include "sydImage.h"

// --------------------------------------------------------------------
namespace sydgui {

  class ImageWidget {
  public:
    ImageWidget();

    bool NewFrame();
    void SetImage(syd::Image::pointer im);

  protected:
    syd::Image::pointer image;
    bool modified;

    sydgui::ListFieldWidget<syd::Patient> patient_list_widget;
    sydgui::ListFieldWidget<syd::Injection> injection_list_widget;
    sydgui::ListFieldWidget<syd::PixelUnit> pixel_unit_list_widget;

    void UpdateListOfPatients();
    void UpdateListOfInjections();
    void UpdateListOfPixelUnits();
  };
}
// --------------------------------------------------------------------

#endif // SYDGUIIMAGEWIDGET_H
