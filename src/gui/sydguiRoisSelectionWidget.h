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

#ifndef SYDGUIROISSELECTIONWIDGET_H
#define SYDGUIROISSELECTIONWIDGET_H

#include "sydgui.h"
#include "sydTimeIntegratedActivityFitOptions.h"

// Boost vector of bool (allow reference)
#include <boost/container/vector.hpp>

// --------------------------------------------------------------------
namespace sydgui {

  class RoisSelectionWidget {
  public:
    RoisSelectionWidget(syd::RoiMaskImage::vector & rois);

    bool NewFrame();
    void SetImage(syd::Image::pointer im);

  protected:
    syd::RoiMaskImage::vector & selected_rois;
    syd::RoiMaskImage::vector all_rois;
    syd::Image::pointer image;
    boost::container::vector<bool> selected;
  };
}
// --------------------------------------------------------------------

#endif // SYDGUIROISSELECTIONWIDGET_H
