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
#include "sydImageBuilder.h"
#include "sydImageHelper.h"

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a Image from a dicom.
  class ImageFromDicomBuilder {

  public:
    /// Constructor.
    ImageFromDicomBuilder();

    /// Set the dicomserie to convert
    void SetInputDicomSerie(syd::DicomSerie::pointer dicom) { dicom_ = dicom; }

    /// If the direction is negative, flip the image
    void SetFlipAxeIfNegativeDirectionFlag(bool b) { flipAxeIfNegativeFlag_ = b; }

    /// Convert the dicom image into an image and insert in the db
    void Update();

    /// Retrieve the inserted image
    syd::Image::pointer GetOutput() { return image_; }

  protected:
    syd::DicomSerie::pointer dicom_;
    syd::Image::pointer image_;
    itk::ImageIOBase::Pointer header_;
    syd::DicomFile::vector dicom_files_; // FIXME TO REMOVE --> will be in DicomSerie
    bool flipAxeIfNegativeFlag_;

    template<class PixelType>
    void UpdateWithPixelType();

  }; // class ImageFromDicomBuilder

#include "sydImageFromDicomBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
