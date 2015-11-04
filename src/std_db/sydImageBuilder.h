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

#ifndef SYDIMAGEBUILDERBASE_H
#define SYDIMAGEBUILDERBASE_H

// syd
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This base class is used to create ImageBuilder class that perform operation on Images
  /// series and images.
  class ImageBuilder {

  public:
    /// Constructor.
    ImageBuilder(syd::StandardDatabase * db) { SetDatabase(db); }

    /// Set the pointer to the database
    void SetDatabase(syd::StandardDatabase * db) { db_ = db; }

    /// Create and insert an empty image (also create associated file)
    syd::Image::pointer InsertNewMHDImage(syd::Patient::pointer patient);

    /// Create and insert a new image and copy fields
    syd::Image::pointer InsertNewMHDImageLike(syd::Image::pointer image);

    /// Create and insert an image, using the dicom (patient)
    syd::Image::pointer InsertNewMHDImage(syd::DicomSerie::pointer dicom);

    /// Create and insert a new RoiMaskImage
    syd::RoiMaskImage::pointer InsertNewMHDRoiMaskImage(syd::Patient::pointer patient, syd::RoiType::pointer roitype);

    /// Update image information from the associated file (pixel type, size, md5 etc)
    void UpdateImageFromFile(syd::Image::pointer image, std::string filename);

    /// Update image information from an itk image, and write the image file in the db (type, size, spacing, md5)
    template<class PixelType>
    void UpdateImage(syd::Image::pointer image, typename itk::Image<PixelType,3>::Pointer & itk_image);

    /// Change the filename to the default one (mv file if already exist)
    void RenameToDefaultFilename(syd::Image::pointer image);

    /// Change the filename to the default one (mv file if already exist)
    void RenameToDefaultFilename(syd::RoiMaskImage::pointer mask);

  protected:
    /// Protected constructor. No need to use directly.
    ImageBuilder();

    /// Pointer to the database
    syd::StandardDatabase * db_;

    /// Create the associated Files
    void InitializeMHDFiles(syd::Image::pointer image);

    /// Raise exception if the image could not be a mhd (2 Files are present etc)
    void CheckMHDImage(syd::Image::pointer image);


  }; // class ImageBuilder

#include "sydImageBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
