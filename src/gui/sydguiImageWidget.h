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
#include "sydguiInjectionListWidget.h"
#include "sydguiPatientListWidget.h"
#include "sydguiPixelUnitListWidget.h"
#include "sydStandardDatabase.h"
#include "sydImage.h"

// --------------------------------------------------------------------
namespace sydgui {

  class ImageWidget {
  public:
    ImageWidget();

    bool NewFrame();//syd::Image::pointer im);
    void SetImage(syd::Image::pointer im);
    syd::Image::pointer GetImage() { return image; }

  protected:
    syd::Image::pointer image;
    bool modified;
    void SetButtons();

    // Widget for list of choices
    sydgui::InjectionListWidget injections_widget;
    sydgui::PatientListWidget patients_widget;
    sydgui::PixelUnitListWidget pixel_unit_widget;

  };

}
// --------------------------------------------------------------------

#endif // SYDGUIIMAGEWIDGET_H
