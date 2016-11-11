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

#ifndef SYDGUIIMAGESSELECTIONWIDGET_H
#define SYDGUIIMAGESSELECTIONWIDGET_H

#include "sydgui.h"
#include "sydStandardDatabase.h"
#include "sydImage.h"

// --------------------------------------------------------------------
namespace sydgui {

  class ImagesSelectionWidget {
  public:
    ImagesSelectionWidget(syd::StandardDatabase * db);

    bool NewFrame();
    const syd::Image::vector & GetImages() const { return selected_images; }

  protected:
    syd::StandardDatabase * db;
    syd::Image::vector all_images;
    syd::Image::vector selected_images;
    syd::Image::pointer selected_image;

    void PrintInfoImage(int i, syd::Image::pointer image);

  };
}
// --------------------------------------------------------------------

#endif // SYDGUIIMAGESSELECTIONWIDGET_H
