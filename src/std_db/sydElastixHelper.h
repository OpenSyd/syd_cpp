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

#ifndef SYDELASTIXHELPER_H
#define SYDELASTIXHELPER_H

// syd
#include "sydElastix.h"

// --------------------------------------------------------------------
namespace syd {


  // Execute "elastix" via a command line and update the Elastix object
  // with the result
  int ExecuteElastix(syd::Elastix::pointer elastix,
                     std::string options,
                     int verbose);

  // Execute "transformix" via a command line and insert an warped image
  syd::Image::pointer InsertTransformixImage(const syd::Elastix::pointer elastix,
                                             const syd::Image::pointer image,
                                             double default_pixel_value,
                                             std::string options,
                                             int verbose);

} // namespace syd
// --------------------------------------------------------------------
#endif
