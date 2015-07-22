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

#ifndef SYDCROPIMAGEBUILDER_H
#define SYDCROPIMAGEBUILDER_H

// syd
#include "sydStandardDatabase.h"
#include "sydImage.h"

// itk
#include <itkImageBase.h>

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a Image. A Image is defined
  /// with an injection, a patient, some tag, some associated dicom
  /// series and images.
  class CropImageBuilder {

  public:
    /// Constructor.
    CropImageBuilder(syd::StandardDatabase * db);

    /// Destructor (empty)
    ~CropImageBuilder() {}

    /// Update an image by cropping according to the given mask
    void CropImageLike(syd::Image::pointer image, const syd::Image::pointer like, bool forceFlag=false);

    /// Crop and image like another (pixeltype known)
    template<class PixelType>
    void CropImageLike(syd::Image::pointer image, syd::Image::pointer like, int interpolationType, PixelType default_pixel);

    /// Crop an image according to a threshold
    void CropImageWithThreshold(syd::Image::pointer image, double threshold);

    /// Crop an image according to a threshold (pixeltype known)
    template<class PixelType>
    void CropImageWithThreshold(syd::Image::pointer image, double lower_threshold);
    // -----------------------------------------------------------


  protected:
    /// Protected constructor. No need to use directly.
    CropImageBuilder();

    /// Set the pointer to the database
    void SetDatabase(syd::StandardDatabase * db) { db_ = db; }

    /// Pointer to the database
    syd::StandardDatabase * db_;

  }; // class CropImageBuilder

#include "sydCropImageBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
