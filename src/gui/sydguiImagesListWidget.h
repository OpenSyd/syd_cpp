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

#ifndef SYDGUIIMAGESLISTWIDGET_H
#define SYDGUIIMAGESLISTWIDGET_H

#include "sydgui.h"
#include "sydStandardDatabase.h"
#include "sydImage.h"

// --------------------------------------------------------------------
namespace sydgui {

  class ImagesListWidget {
  public:
    ImagesListWidget(syd::Image::vector im);

    bool NewFrame();
    void SetImages(syd::Image::vector im);

  protected:
    syd::Image::vector images;
    std::set<int> ids; // list of different patient ids
    syd::Patient::vector patients; // list of patient
    std::map<int, int> nb; // nb of images by patient 

  };
}
// --------------------------------------------------------------------

#endif // SYDGUIIMAGESLISTWIDGET_H
