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

#ifndef SYDROIMASKIMAGEBUILDER_H
#define SYDROIMASKIMAGEBUILDER_H

// syd
#include "sydImageBuilder.h"

// itk
#include <itkImageBase.h>

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a Image. A Image is defined
  /// with an injection, a patient, some tag, some associated dicom
  /// series and images.
  class RoiMaskImageBuilder: public syd::ImageBuilder {

  public:
    /// Constructor.
    RoiMaskImageBuilder(syd::StandardDatabase * db):ImageBuilder(db) { }

    /// Create & Insert a RoiMaskImage from an image.mhd
    syd::RoiMaskImage::pointer
    NewRoiMaskImage(const syd::DicomSerie::pointer & dicom,
                    const syd::RoiType::pointer & roitype,
                    const std::string & filename);

  }; // class RoiMaskImageBuilder

} // namespace syd
// --------------------------------------------------------------------

#endif
