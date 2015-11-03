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

#ifndef SYDIMAGEFROMDICOMBUILDER_H
#define SYDIMAGEFROMDICOMBUILDER_H

// syd
#include "sydImageBuilderBase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a Image. A Image is defined
  /// with an injection, a patient, some tag, some associated dicom
  /// series and images.
  class ImageFromDicomBuilder: public syd::ImageBuilderBase {

  public:
    /// Constructor.
    ImageFromDicomBuilder(syd::StandardDatabase * db):ImageBuilderBase(db) {}

    /// Main function: convert a dicom to a mhd image
    syd::Image::pointer InsertImageFromDicom(const syd::DicomSerie::pointer dicom);

  }; // class ImageFromDicomBuilder

} // namespace syd
// --------------------------------------------------------------------

#endif
