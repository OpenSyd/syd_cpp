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
#include "sydStandardDatabase.h"
#include "sydImage.h"
//#include "sydImageUtils.h"

// itk
#include <itkImage.h>

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a Image. A Image is defined
  /// with an injection, a patient, some tag, some associated dicom
  /// series and images.
  class ImageFromDicomBuilder {

  public:
    /// Constructor.
    ImageFromDicomBuilder(syd::StandardDatabase * db);

    /// Destructor (empty)
    ~ImageFromDicomBuilder() {}

    /// Main function: convert a dicom to a mhd image
    syd::Image::pointer CreateImageFromDicom(const syd::DicomSerie::pointer dicom);

    /// Update image information from this itk image (type, size, spacing)
    template<class PixelType>
    void UpdateImageInfo(syd::Image::pointer image,
                         typename itk::Image<PixelType,3>::Pointer & itk_image,
                         bool computeMD5Flag);

    /// Read itk image from a DicomSerie
    template<class PixelType>
    typename itk::Image<PixelType,3>::Pointer
    ReadImage(const syd::DicomSerie::pointer dicom);

  protected:
    syd::StandardDatabase * db_;

  }; // class ImageFromDicomBuilder

#include "sydImageFromDicomBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
