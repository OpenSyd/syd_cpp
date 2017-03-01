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

#ifndef SYDFITIMAGESHELPER_H
#define SYDFITIMAGESHELPER_H

// syd
#include "sydFitImages.h"
#include "sydImageHelper.h"

// --------------------------------------------------------------------
namespace syd {

  /// Create a new fit image output for a given FitImages.
  syd::Image::pointer NewOutputFitImages(const syd::FitImages::pointer tia,
                                         const std::string fit_output_name);

  /// Create the fit model at the given pixel coordinate
  syd::FitTimepoints::pointer
    NewFitTimepointsAtPixel(const syd::FitImages::pointer tia,
                            const std::vector<double> & pixel);

} // namespace syd
// --------------------------------------------------------------------

#endif
