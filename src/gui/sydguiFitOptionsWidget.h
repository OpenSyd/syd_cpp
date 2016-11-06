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

#ifndef SYDGUIFITOPTIONSWIDGET_H
#define SYDGUIFITOPTIONSWIDGET_H

#include "sydgui.h"
#include "sydTimeIntegratedActivityFitOptions.h"

// --------------------------------------------------------------------
namespace sydgui {

  class FitOptionsWidget {
  public:
    FitOptionsWidget(syd::TimeIntegratedActivityFitOptions & options);

    bool NewFrame();
    void SetImage(syd::Image::pointer im);

  protected:
    syd::TimeIntegratedActivityFitOptions & options;
    syd::Image::pointer image;

    bool f2_model;
    bool f3_model;
    bool f4_model;
    bool f4a_model;

  };
}
// --------------------------------------------------------------------

#endif // SYDGUIFITOPTIONSWIDGET_H
