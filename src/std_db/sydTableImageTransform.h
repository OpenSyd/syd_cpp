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

#ifndef SYDTABLEIMAGETRANSFORM_H
#define SYDTABLEIMAGETRANSFORM_H

// syd
#include "sydImageTransform.h"
#include "sydTableFile.h"

// --------------------------------------------------------------------
namespace syd {

  /// Specialization for ImageTransform: data are in folder named 'transform'
  template<>
  std::string ComputeRelativeFolder(const syd::Database * db, const syd::ImageTransform & e);

  /// Specialization for ImageTransform: return the path of the linked file
  template<>
  std::string GetRelativePath<syd::ImageTransform>(const syd::Database * db, const syd::ImageTransform & e);

  /// Create a new ImageTransform
  template<class ImageTransform>
  void InsertNewImageTransform(ImageTransform & t, syd::Database * db,
                               syd::Image & fixed_image, syd::Image & moving_image, std::string & config_filename);

#include "sydTableImageTransform.txx"

} // end namespace
// --------------------------------------------------------------------

#endif