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

    /// Set the mhd pixel_type to read and write
    void SetImagePixelType(std::string pixel_type);

    /// Set the dicomserie to convert
    void SetInputDicomSerie(syd::DicomSerie::pointer dicom);

    /// Convert the dicom image into an image and insert in the db
    void Update();

    /// Retrieve the inserted image
    syd::Image::pointer GetOutput() { return image_; }

  protected:
    syd::DicomSerie::pointer dicom_;
    syd::Image::pointer image_;
    itk::ImageIOBase::Pointer header_;
    std::string user_pixel_type_;

    template<class PixelType>
    void WriteMhd(std::string temp_filename);

  }; // class ImageFromDicomBuilder

#include "sydImageFromDicomBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
